/// 2024. 01. 10 김상준
/// 에디터에서 사용하게 될 입력 처리 KeyCode

#pragma once

namespace application
{
	namespace editor
	{
		/// Yunuty 와 동일한 KeyCode 를 사용하도록 함
        /// 단, Mouse 관련 부분을 따로 빼내어 분리함
		enum class KeyCode : unsigned char
		{
            NoKey = 0,
            ESC,
            LeftArrow,
            RightArrow,
            UpArrow,
            DownArrow,
            Space,
            Return,
            Control,
            //LShift,
            Shift,
            Alt,
            Tab,
            Backspace,
            Delete,
            Insert,
            Home,
            End,
            PgUp,
            PgDn,
            //MouseLeftClick,
            //MouseMiddleClick,
            //MouseRightClick,
            A = 20, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_0,
            NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4, NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9, NUMPAD_0,
            Plus, Comma, Minus, Perido, Slash, BackSlash
		};

        enum class MouseCode
        {
            None = 0,
            Left = 20,
            Middle,
            Right,
        };
	}
}
