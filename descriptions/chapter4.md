# 4. 인터프리터

지금까지 소스 코드의 문자열을 분석하는 어휘 분석과 구조를 분석하는 구문 분석을 학습하였다.

즉 소스 코드의 문자열로부터 토큰 리스트를 만드는 방법을 학습하였고, 토큰 리스트로부터 구문 트리를 만드는 방법을 학습했다.

컴파일 과정을 두 부분으로 나누면 전단부와 후단부로 나눌 수 있는데, 구문 분석의 결과물인 구문 트리는 다음 그림과 같이 전단부와 후단부를 잇는 매개체다.

![alt text](./images/4.png)

인터프리터란 소스 코드를 실행하는 프로그램을 말한다. 더 정확하게 표현하면 인터프리터는 구문 트리를 실행하는 프로그램이다. 즉 구문 트리를 순회하며 노드를 실행한다.

## 4.1 구문 트리 순회

인터프리터는 구문 트리를 순회하며 실행하는 프로그램이므로, 인터프리터를 만들기 위해 가장 먼저 해야 할 것은 구문 트리를 순회할 방법을 결정하는 것이다.

유랭 컴파일러는 단순하게 가상 함수를 사용해서 구문 트리를 순회한다. 구문 분석에서 작성했던 노드들에 가상 함수를 추가하자. 문 노드를 순회할 때는 interpret() 가상 함수를 호출해 구문 트리를 순회한다.
```cpp
struct Statement {
  virtual auto interpret() -> void = 0;
}
```

부모 문 노드에 순수 가상 함수를 선언했으므로, 부모 문 노드를 상속받는 모든 문 노드에 interpret() 함수를 선언한다.

식도 마찬가지이다. 다음과 같이 부모 식 노드에 interpret() 라는 순수 가상 함수를 선언하는데, 식은 결과값이 있으므로 반환값의 데이터 타입을 any로 선언한다. 식 노드를 순회할 때도 interpret() 가상 함수를 호출해 구문 트리를 순회한다.

마찬가지로 모든 식 노드를 상속받는 모든 식 노드에 interpret() 함수를 선언한다.
```cpp
struct Expression {
  virtual auto interpret() -> any = 0;
}
```

이후 Interpreter.cpp 파일에 작성한 함수들의 정의를 채워간다.

## 4.2 구문 트리 실행기

인터프리터는 구문 트리를 순회하며 실행하는 프로그램을 뜻하므로 구문 트리의 루트 노드를 입력받는다. 구문 트리를 실행하는 interpret() 함수를 Main.h 에 선언한다.
```cpp
auto interpret(Program*) -> void;
```

main() 에서는 구문 트리의 루트 노드를 인자로 넘겨 interpret() 함수를 호출한다.
```cpp
vector<Token> tokenList = scan(sourceCode);
Program* syntaxTree = parse(tokenList);
interpret(syntaxTree);
```

## 4.2.1 엔트리 포인트 함수 호출

구문 트리의 루트 노드이자 소스 코드의 선언 영역을 표현하는 프로그램 노드는 함수의 정의들을 포함한다. 유랭은 함수를 정의한 순서와 관계없이 호출할 수 있으므로 다음과 같이 functionTable이라는 전역 변수를 선언한다. functionTable은 함수의 이름과 함수의 노드를 키와 값으로 가진다. interpret() 함수에서는 프로그램 노드에 포함된 함수 노드들을 functionTable 변수에 등록한다.
```cpp
static map<string, Function*> functionTable;

auto interpret(Program* program) -> void {
  for (Function* node: program->functions) {
    functionTable[node->name] = node;
  }
}
```

정의된 함수들을 호출할 수 있도록 전역 변수에 등록했으니 이제 엔트리 포인트 함수를 실행해야 한다. 유랭의 엔트리 포인트는 main() 함수이므로 functionTable에서 키가 main으로 등록된 함수 노드를 찾아 interpret() 함수를 호출한다.
```cpp
if (functionTable.count("main") == ) {
  return;
}
functionTable["main"]->interpret();
```

## 4.2.2 함수

함수의 실행은 단순히 본문의 노드들을 순회하는 것이 전부다. 다음과 같이 함수 노드의 문 리스트를 순회하며 interpret() 함수를 호출함으로써 함수가 실행된다.
```cpp
auto Function::interpret() -> void {
  for (Statement* node: block) {
    node->interpret();
  }
}
```

## 4.2.3 print문

인터프리터는 소스 코드에 작성된 코드의 실행 흐름대로 노드들을 방문하므로 모든 모드의 interpret() 함수의 내용을 작성하지 않아도 구문 트리를 실행해 볼 수 있다. 콘솔에 "Hello, World!"를 출력하는 소스 코드를 실행할 수 있도록 해보자.

print문의 실행은 print문 노드가 가진 식 노드들을 순회하며 interpret() 함수를 호출하는 것으로 print 문의 실행은 끝이다.
```cpp
auto Print::interpret() -> void {
  for (Expression* node: arguments) {
    any value = node->interpret();
    cout << value;
  }
  if (lineFeed) cout << endl;
}
```

## 4.2.4 데이터 타입

유랭은 동적 타입 언어이므로 인터프리터에서 데이터 타입을 표현하기 위해 C++17에서 지원하는 any를 사용한다. any는 마치 동적 타입 언어처럼 어떤 타입의 데이터든 저장할 수 있도록 해준다. 

그런데 C++ 자체는 정적 타입 언어이므로 any 타입에 저장된 값을 사용하려면 어떤 타입의 데이터가 저장되어 있는지는 알아야 한다. any에 저장된 데이터 타입을 확인하는 것과 저장된 값의 데이터 타입으로 캐스팅하는 것은 매번 반복되므로 보조 함수를 만들어 사용한다.

에이터 타입과 관련된 보조 함수들은 Datatype.cpp 파일에 작성한다. is로 시작하는 함수는 any 타입에 들어있는 값의 타입이 무엇인지 확인하고, to로 시작하는 함수들은 any 타입에 들어있는 값을 캐스팅한다.

```cpp
auto isString(any value) -> bool {
  return value.type() == typeid(string);
}

auto toString(any value) -> string {
  return any_cast<string>(value);
}
```

식 노드의 interpret() 함수가 반환하는 값의 데이터 타입은 any다. 콘솔에 any 타입의 값을 출력하기 위해서는 저장된 값의 데이터 타입을 알아야 하므로 실제로 콘솔에 식 노드의 반환값을 출력하는 함수는 다음과 같다.
```cpp
auto operator<<(ostream& stream, any& vaue) -> ostream& {
  if (isString(value)) stream << toString(value);
  ...
}
```

## 4.2.5 문자열 리터럴

이제 문자열 리터럴 노드의 interpret() 함수만 작성하면 "Hello, World!" 프로그램을 실행할 수 있다. 문자열 리터럴 노드에서는 노드가 가진 문자열 값을 반환하기만 하면 된다.

Function 노드에서는 Statement 노드를 순회하며 interpret() 함수를 호출해주면 된다.
```cpp
auto StringLiteral::interpret() -> void {
  return value;
}

auto Function::interpret() -> void {
  for (Statement* node : block) {
    node->interpret();
  }
}
```

```cpp
func main() {
  printLine("Hello World!");
  // printLine("Hi");
  /*
  printLine("Hi");
  printLine("Hi");
  */
}
```

![alt text](./images/5.png)

> // 한줄 주석, /* */ 여러줄 주석 기능도 추가하였다.

> 리눅스에서는 파일 복사시 수정시간 갱신하는데 윈도우에서는 수정시간 유지된다.
> 그래서 make 가 새롭게 빌드 안해준다 !!! 주의.

## 4.2.6 산술 연산

산술 연산자 노드는 왼쪽 식 노드와 오른쪽 식 노드를 멤버로 가진다. 연산을 하기 위해 양쪽 식 노드를 순회해서 두 피연산자의 값을 구한다.
```cpp
auto Arithmetic::interpret() -> any {
  any lValue = lhs->interpret();
  any rValue = rhs->interpret();
}
```

두 피연산자의 값을 구한 후에 할 일은 연산의 결과값을 반환하는 것이다. 연산자의 종류와 두 피연산자의 데이터 타입에 따라 연산을 하고 결과값을 반환한다. 예를 들어 연산자가 덧셈이고 두 피연산자의 데이터 타입이 숫자인 경우에는 덧셈을 해서 결과값을 반환한다.

마찬가지로 연산자가 덧셈이고 두 피연산자의 데이터 타입이 문자열이라면 다음과 같이 문자열 덧셈을 한다.
```cpp
if (kind == Kind::Add && isNumber(lValue) && isNumber(rValue)) {
  return toNumber(lValue) + toNumber(rValue);
}
if (kind == Kind::Add && isString(lValue) && isString(rValue)) {
  return toString(lValue) + toString(rValue);
}
```

![alt text](./images/6.png)

## 4.2.7 논리 연산

논리 연산자는 산술 연산자와 마찬가지로 두 개의 피연산자를 가지지만, 단락 평가가 되도록 interpret() 함수의 내용을 작성해야 한다.

다음 코드는 or 연산자 노드의 interpret() 함수이다.
```cpp
auto Or::interpret() -> any {
  return isTrue(lhs->interpret()) ? true : rhs->interpret();
}
```

다음 코드는 and 연산자 노드의 interpret() 함수이다.
```cpp
auto And::interpret() -> any {
  return isFalse(lhs->interpret()) ? false : rhs->interpret();
}
```

```cpp
func main() {
  printLine("Hello World!");
  print("1 + 2 = ");
  printLine(1 + 2);
  printLine(true or "Hello World!");
  printLine(false or "Hello World!");
  printLine(true and "Hello World!");
  printLine(false and "Hello World!");
}
```

![alt text](./images/7.png)