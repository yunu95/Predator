using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.ComponentModel.Design;
using System.IO;
using EnvDTE;
using EnvDTE80;
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using Task = System.Threading.Tasks.Task;

namespace YunusVSExtension
{
    /// <summary>
    /// Command handler
    /// </summary>
    internal sealed class Command1
    {
        /// <summary>
        /// Command ID.
        /// </summary>
        public const int CommandId = 0x0101;

        /// <summary>
        /// Command menu group (command set GUID).
        /// </summary>
        public static readonly Guid CommandSet = new Guid("5b644325-e264-4f85-8001-b80d8e816117");

        /// <summary>
        /// VS Package that provides this command, not null.
        /// </summary>
        private readonly AsyncPackage package;

        /// <summary>
        /// Initializes a new instance of the <see cref="Command1"/> class.
        /// Adds our command handlers for menu (commands must exist in the command table file)
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        /// <param name="commandService">Command service to add command to, not null.</param>
        private Command1(AsyncPackage package, OleMenuCommandService commandService)
        {
            this.package = package ?? throw new ArgumentNullException(nameof(package));
            commandService = commandService ?? throw new ArgumentNullException(nameof(commandService));

            var menuCommandID = new CommandID(CommandSet, CommandId);
            var menuItem = new MenuCommand(this.Execute, menuCommandID);
            commandService.AddCommand(menuItem);
        }

        /// <summary>
        /// Gets the instance of the command.
        /// </summary>
        public static Command1 Instance
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the service provider from the owner package.
        /// </summary>
        private Microsoft.VisualStudio.Shell.IAsyncServiceProvider ServiceProvider
        {
            get
            {
                return this.package;
            }
        }

        /// <summary>
        /// Initializes the singleton instance of the command.
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        public static async Task InitializeAsync(AsyncPackage package)
        {
            // Switch to the main thread - the call to AddCommand in Command1's constructor requires
            // the UI thread.
            await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync(package.DisposalToken);

            OleMenuCommandService commandService = await package.GetServiceAsync(typeof(IMenuCommandService)) as OleMenuCommandService;
            Instance = new Command1(package, commandService);
        }

        /// <summary>
        /// This function is the callback used to execute the command when the menu item is clicked.
        /// See the constructor to see how the menu item is associated with this function using
        /// OleMenuCommandService service and MenuCommand class.
        /// </summary>
        /// <param name="sender">Event sender.</param>
        /// <param name="e">Event args.</param>
        private void Execute(object sender, EventArgs e)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            string message = string.Format(CultureInfo.CurrentCulture, "Inside {0}.MenuItemCallback()", this.GetType().FullName);
            string title = "Command1";

            var dte = (DTE2)Package.GetGlobalService(typeof(DTE));
            Array selectedItems = (Array)dte.ToolWindows.SolutionExplorer.SelectedItems;
            if (selectedItems == null || selectedItems.Length == 0) return;

            UIHierarchyItem item = (UIHierarchyItem)selectedItems.GetValue(0);
            ProjectItem projectItem = (ProjectItem)item.Object;
            string filterName = projectItem.Name;

            string projectPath = projectItem.ContainingProject.FullName;
            string projectDir = Path.GetDirectoryName(projectPath);
            string headerFilePath = Path.Combine(projectDir, filterName + ".h");
            string sourceFilePath = Path.Combine(projectDir, filterName + ".cpp");

            if (File.Exists(headerFilePath) || File.Exists(sourceFilePath))
            {
                return;
            }
            string headerContents = null;
            string cppContents = null;
            if (filterName.EndsWith("Type"))
            {
                headerContents = "#pragma once\n\nenum class " + filterName + "\n{\n    NONE,\n};";
            }
            else
            {
                headerContents = "#pragma once\n\nclass " + filterName + "\n{\n    public:\n    private:\n};";
                cppContents = "#include \"" + filterName + ".h\"";
            }
            if (headerContents != null)
            {
                File.WriteAllText(headerFilePath, headerContents);
                projectItem.ProjectItems.AddFromFile(headerFilePath);
            }
            if (cppContents != null)
            {
                File.WriteAllText(sourceFilePath, cppContents);
                projectItem.ProjectItems.AddFromFile(sourceFilePath);
            }

            //projectItem.ContainingProject.ProjectItems.AddFromFile(headerFilePath);
            //projectItem.ContainingProject.ProjectItems.AddFromFile(sourceFilePath);

        }
    }
}
