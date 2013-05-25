#define DEFAULT_CODE_LENGTH 512
#define SCALE 4

class chainCode{
    public:
        char* code;
        int   length;       

        chainCode();
        ~chainCode();
        void add(char c);
        chainCode* postProcess();
        void printSelf();
};


