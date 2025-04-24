# 5. 코드 생성

코드 생성은 목적 코드를 생성하는 과정이다. 목적 코드는 소스 코드와 반대되는 개념으로, 소스 코드는 사람이 읽을 수 있는 문자열 형태의 코드인 반면에 목적 코드는 기계가 읽을 수 있는 바이너리 형태의 코드다.

목적 코드를 생성하는 주된 이유는 실행 속도 때문이다. 구문 트리와 같은 비선형 자료구조의 순회는 배열이나 리스트와 같은 선형 자료 구조의 순회에 비해 느리다. 

따라서 구문 트리가 담고 있는 내용을 그대로 선형 구조로 표현한다면 실행 속도를 높일 수 있따.

인터프리터가 일관된 규칙에 따라 노드들을 순서대로 순회하고 실행했으므로, 구문 트리를 선형 구조로 표현하는 것도 가능하다. 코드 생성은 비선형 구조인 구문 트리의 내용을 선형 구조로 표현하는 것이다.

## 5.1 목적 코드와 명령어

소스코드가 프로그래밍 언어로 작성한 코드인 반면에 목적 코드는 기계가 제공하는 명령어로 작성한 코드다. 안드로이드용으로 만든 프로그램이 다른 기계에서 동작하지 않는 이유는 기계들이 저마다 서로 다른 명령어를 제공하기 때문이다.

이 책에서는 가상머신을 대상으로 목적 코드를 생성한다. 가상머신은 물리 머신과 반대되는 개념으로 하드웨어 동작을 소프트웨어로 구현한 프로그램을 말한다.

자바 컴파일러가 자바 가상머신을 대상으로 목적 코드를 생성하듯이, 유랭 컴파일러는 유랭 가상머신을 대상으로 목적 코드를 생성한다.

그러면 유랭 가상머신의 명령어를 정의하자.
```cpp
enum class Instruction {
  Exit,
  Call, Alloca, Return,
  Jump, ConditionJump,
  Print, PrintLine,

  LogicalOr, LogicalAnd,
  Multiply, Divide, Modulo,
  Equal, NotEqual,
  LessThan, GreaterThan,
  LessOrEqual, GreaterOrEqual,
  Add, Subtract,
  Absolute, ReverseSign,

  GetElement, SetElement,
  GetGlobal, SetGlobal,
  GetLocal, SetLocal,

  PushNull, PushBoolean,
  PushNumber, PushString,
  PushArray, PushMap,
  PopOperand
};
```
위에 열거형으로 정의된 명령어들을 보면 대부분 설명이 따로 필요 없을 만큼 익숙한 단어들이 나열된 것을 볼 수 있다. 물리 머신에 비해 가상머신의 명령어는 높은 레벨로 추상화가 가능하기 때문이다.

- Exit(프로그램 종료)
- Call(함수 호출): 호출하는 함수에 넘길 인자의 개수를 인자로 가진다
- Alloca(메모리 할당): 호출된 함수의 지역 변수들의 값을 저장할 메모리 공간을 확보한다. 지역 변수 개수를 인자로 가진다.
- Return(함수 종료): 함수의 결과값을 반환하며 함수를 종료한다.
- Jump(분기): C++의 goto문과 같다. 점프할 코드의 주소를 인자로 가진다.
- ConditionJump(조건 분기): if문과 for문의 조건식의 결과가 거짓인 경우에 분기한다. 점프할 코드의 주소를 인자로 가진다.
- Print, PrintLine(콘솔 출력): 피연산자 스택에서 값을 꺼내 콘솔에 출력한다. 콘솔에 출력할 값의 개수를 인자로 가진다.
- LogicalOr, LogicalAnd(논리 연산): 순서대로 논리or, and 연산자이지만 단락 평가를 하므로 조건 분기 명령처럼 동작한다. 점프할 코드의 주소를 인자로 가진다.
- Add, Subtract, Multiply, Divide, Modulo(산술 연산): 순서대로 + - * / % 연산자다.
- Equal, NotEqual, LessThan, GreaterThan, LessOrEqual, GreaterOrEqual(비교 연산): 순서대로 ==, !=, <, >, <=, >= 연산자다.
- Absolute, ReverseSign(단항 연산): 순서대로 +, - 연산자다. 절대값 연산과 부호 반전 연산을 한다.
- GetElement, SetElement(원소 연산): 순서대로 원소값의 참조와 수정 연산을 하는 맵과 배열의 [] 인덱스 연산이다.
- GetGlobal, SetGlobal(전역 변수 연산): 순서대로 전역 변수 값의 참조와 수정 연산을 한다. GetGlobal은 전역 변수의 값을 피연산자 스택에 넣는다. SetGlobal은 대입 연산자와 같다. 전역 변수의 이름을 인자로 가진다.
- GetLocal, SetLocal(지역 변수 연산): 순서대로 지역 변수 값의 참조와 수정 연산을 한다. GetLocal은 지역 변수의 값을 피연산자 스택에 넣는다. SetLocal은 대입 연산자와 같다. 지역 변수의 오프셋을 인자로 가진다.
- PushNull, PushBoolean, PushNumber, PushString(프리미티브 타입 리터럴 연산): 순서대로 널, 불리언, 숫자, 문자열 값을 피연산자 스택에 넣는다. 데이터 타입에 따른 리터럴 값을 인자로 갖는다.
- PushArray, PushMap(레퍼런스 타입 리터럴 연산): 순서대로 배열과 맵을 생성해 피연산자 스택에 넣는다. 생성할 배열이나 맵의 원소의 개수를 인자로 가진다. 원소의 개수만큼 피연산자 스택에서 값을 꺼내 배열이나 맵을 생성한다.
- PopOperand(피연산자 스택 연산): 피연산자 스택에서 값을 꺼내 버린다.

명령어는 역할에 따라 인자를 가지므로 명령어와 인자를 묶어 다음과 같이 목적 코드를 표현할 구조체를 정의한다.
```cpp
struct Code {
  Instruction instruction;
  any operand;
};
```

코드 생성은 구문 트리를 순회하며 노드들의 내용을 목적 코드로 작성한다. 따라서 인터프리터에서와 같이 구문 트리를 순회하기 위한 가상 함수가 필요하다. 인터프리터에서 각 노드에 interpret() 라는 함수를 정의했던 방식과 동일하다. 먼저 부모 문 노드와 부모 식 노드에 generate() 라는 순수 가상 함수를 선언한다.
```cpp
struct Statement {
  virtual auto generate() -> void = 0;
};

struct Expression {
  virtual auto generate() -> void = 0;
}
```

이후 부모 문 노드와 식 노드를 상속하는 모든 노드에 generate() 함수를 선언하고, Generator.cpp 파일에 정의를 작성한다.

## 5.2 코드 생성기

코드 생성은 구문 트리의 내용을 목적 코드로 작성하는 것이고, 목적 코드는 대상 머신의 명령어로 작성된 코드다. 코드 생성기는 목적 코드를 생성하는 프로그램을 뜻하므로 구문 트리의 루트 노드를 입력받아 목적 코드를 출력한다.

코드를 생성하는 generate() 함수를 Main.h 파일에 선언한다.
```cpp
auto generate(Program*) -> tuple<vector<Code>, map<string, size_t>>;
```

main() 함수에서는 다음과 같이 구문 트리의 루트 노드를 인자로 넘겨 generate() 함수를 호출한다.
```cpp
string sourceCode = R""""(
  func main() {
    print("Hello, World!");
  }
)"""";
vector<Token> tokenList = scan(sourceCode);
Program* syntaxTree = parse(tokenList);
tuple<vector<Code>, map<string, size_t>> objectCode = generate(syntaxTree);
printObjectCode(objectCode);
```

다음의 출력 결과는 printObjectCode() 함수가 출력한 내용이자, 이 장에서 만드는 코드 생성 프로그램의 결과다.
```
FUNCTION    ADDRESS
--------------------
main        3

ADDR INSTRUCTION    OPERAND
----------------------------
0    GetGlobal      "main"
1    Call           [0]
2    Exit
3    Allca          [0]
4    PushString     "Hello, World!"
5    Print          [1]
6    Return
```