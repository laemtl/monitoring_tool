
#include <sys/types.h>

class Hashable
{

    public:
        u_int32_t hash();
        int compare(Hashable* elem);
        void update();
        ~Hashable();
};