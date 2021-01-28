#include <string>

using namespace std;

class UUOS {
    public:
        UUOS();
        virtual ~UUOS();
        virtual void set_log_level(string& logger_name, int level);
    private:

};
