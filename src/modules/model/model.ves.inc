#define QUOTE(...) #__VA_ARGS__
static const char* modelModuleSource = QUOTE(

foreign class Model
{
}

);