#include "Main.h"

int main() {
    string sourceCode = R""""(
        function main() {
            printLine 'Hello, World!';
            printLine 1 + 2 * 3;
        }
    )"""";
    vector<Token> tokenList = scan(sourceCode);
    Program* syntaxTree = parse(tokenList);

    printSyntaxTree(syntaxTree);
    return 0;
}
