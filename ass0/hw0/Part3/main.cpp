#include <cassert>
#include "file_reader.h"

using namespace std;

int main(int argc, const char* argv[]) {
    // Should only be one argument, the input file
    assert (argc == 2);
    // Print the transformed objects!
    vector<object *> objects = get_objects(argv[1]);
    output_transformed_objects(objects);

    // Free memory
    for (object *o : objects)
        delete o;
}
