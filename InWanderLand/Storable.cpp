#include "Storable.h"

namespace Application
{
    namespace Editor
    {
        json Storable::Encoding() const
        {
            json data;

            PreEncoding(data);
            PostEncoding(data);

            return data;
        }

        bool Storable::Decoding(const json& data)
        {
            if (!PreDecoding(data))
            {
                return false;
            }
            
            if (!PostDecoding(data))
            {
                return false;
            }

            return true;
        }
    }
}
