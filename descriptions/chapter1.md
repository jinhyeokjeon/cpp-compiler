# 1. 시작하며

## 1.1 프로그래밍 언어

컴파일러 만들기의 진입 장벽을 낮추기 위해 이 책의 예제 컴파일러는 유랭이라는 프로그래밍 언어를 컴파일한다. 유랭은 이 책을 위해 저자가 디자인한 언어로 단순한 문법과 필수적인 기능들만을 사양으로 한다. 이 절에서는 컴파일러 구현에 필요한 유랭의 문법에 대해 알아본다.

유랭의 엔트리 포인트는 main() 함수다. print문으로 콘솔에 값을 출력할 수 있다.
```c
func main() {
  print("Hello, World!");
}
```

변수는 var 키워드로 선언하며 기본 데이터 타입에는 불리언, 숫자, 문자열이 있다. 숫자는 정수와 실수를 포함하고, 문자열은 쌍따옴표를 사용한다. print문은 콤마를 구분자로 사용하여 여러 개의 값을 출력할 수 있다.
```c
func main() {
  var t = true;
  var f = false;
  var i = 13;
  var d = 1.3;
  var s = "Hello, World!";
  print(t, " ", f, " ", i, " ", d, " ", s);
}

true false 13 1.3 Hello, World!
```

for문은 다음과 같이 사용한다.
```c
func main() {
  var n = 5;
  for(var i = 0; i < n; i = i + 1) {
    for(var j = 0; j < n; j = j + 1) {
      print(i * n + j + 1, " ");
    }
    printLine();
  }
}

1 2 3 4 5
6 7 8 9 10
11 12 13 14 15
16 17 18 19 20
21 22 23 24 25
```

if문은 다음과 같이 사용한다. if문의 본문에서 사용할 수 있는 continue문과 break문의 특성은 일반적인 프로그래밍 언어들과 같다.

```c
func main() {
  for (var i = 1; true; i = i + 1) {
    if (i > 10) {
      break;
    }
    elif (i % 2 == 0) {
      continue;
    }
    else {
      print(i, " ");
    }
  }
}

1 3 5 7 9
```

함수는 다음과 같이 정의하고 호출한다. 재귀적으로 호출할 수 있으며 return 문으로 값을 반환한다. 함수를 정의한 순서와 관계없이 호출할 수 있다. 명시적으로 값을 반환하지 않으면 null이 반환된다.

```c
func main() {
  print(fibonacci(13));
}

func fibonacci(var n) {
  if (n < 2) {
    return n;
  }
  return fibonacci(n - 2) + fibonacci(n - 1);
}

233
```

배열은 다음과 같이 사용한다. 내장함수 push()와 pop()을 사용해 배열의 끝에 값을 추가하거나 마지막 원소를 제거할 수 있다. [] 인덱스 연산자로 값을 참조하거나 수정할 수 있다. 내장함수 length()로 원소의 개수를 구할 수 있다.
```c
func main() {
  var a = [1, 2];
  for (var i = 3; i <= 5; i = i + 1) {
    push(a, i);
  }
  printLine(a);
  pop(a);
  a[length(a) - 1] = 5;
  printLine(a);
}

[1 2 3 4 5]
[1 2 3 5]
```

맵은 다음과 같이 사용한다. [] 인덱스 연산자로 값을 추가하거나 수정할 수 있다. 내장함수 erase()로 값을 제거할 수 있다. 배열과 마찬가지로 내장함수 length()로 원소의 개수를 구할 수 있다.
```c
func main() {
  var m = {'a': 1};
  printLine(m);
  m['a'] = 2;
  printLine(m);
  m['b'] = 3;
  printLine(m);
  erase(m, 'a');
  printLine(m);
}

{ a:1 }
{ a:2 }
{ a:2 b:3}
{ b:3 }
```

배열과 맵은 레퍼런스 타입으로 함수의 인자와 반환값으로 자유롭게 사용할 수 있다. 또한 다음과 같이 JSON처럼 배열 안에 맵을 원소로 사용하거나 맵의 원소로 배열을 사용할 수도 있다.

```c
func main() {
  var array = [{"key": [1, 2, 3]}, 1, true, "string"];
  test(array);
  printLine(array[0]["key"][2]);
}

func test(var array) {
  array[0]["key"][2] = 7;
}

7
```

## 1.2 컴파일러

컴파일러는 코드를 입력받아 코드를 출력하는 프로그램이다. 컴파일러 입장에서 입력받는 코드를 소스코드라 하고, 출력하는 코드를 목적코드라 한다.

즉 프로그래머가 컴파일러에 입력하는 코드가 소스코드이고, 컴파일러부터 출력받는 코드가 목적코드다. 

일반적으로 소스코드는 프로그래밍 언어로 작성한 문자열 형태의 코드고, 목적코드는 바이너리 형태의 코드다. 

컴파일러는 소스코드를 목적코드로 번역하는 프로그램이다.

```
소스 코드 -> 컴파일러 -> 목적코드
```
소스코드를 목적 코드로 번역하는 과정 또는 행위를 컴파일이라 한다. 컴파일은 세 단계로 나뉜 동작을 순차적으로 실행하는 과정이다. 첫 번째 단계를 어휘 분석, 두 번째 단계를 구문 분석, 세 번째 단계를 코드 생성이라 한다.

```
소스 코드 -> 어휘 분석 -> 구문 분석 -> 코드 생성 -> 목적 코드
```

컴파일러가 소스 코드를 번역하는 프로그램이라면 인터프리터는 소스 코드를 실행하는 프로그램이다.

```
소스 코드 -> 인터프리터 -> 실행 결과
```

인터프리터 또한 컴파일러와 동일하게 어휘 분석과 구문 분석의 과정을 거친다. 컴파일러는 **목적 코드**를 출력하고, 인터프리터는 **실행 결과**를 출력한다.

Java 컴파일러가 Java로 작성한 소스 코드를 Java 바이트코드로 컴파일하듯이, 유랭 컴파일러는 유랭으로 작성한 소스 코드를 유랭 바이트코드로 컴파일한다. 또한 Java 바이트코드를 실행하기 위해 Java 가상머신이 필요하듯이 유랭 바이트 코드를 실행하기 위해서는 유랭 가상머신이 필요하다.

```
목적 코드 -> 가상 머신 -> 실행 결과
```

각 프로젝트의 main.cpp 파일에 있는 main() 함수들은 모두 다음과 같은 코드로부터 시작한다.

```cpp
auto main() -> void {
  string sourceCode = R""""(
    function main() {
      print("Hello, World!"); 
    } 
  )"""";
}
```

위 코드에서 문자열 변수 sourceCode는 유랭으로 작성한 소스 코드를 담고 있다. R""""(와 )"""" 사이에 있는 문자열이 유랭으로 작성한 소스 코드다.

> R"delimiter(문자열 내용)delimiter" : \n \t 같은 escape 문자를 인식하지 않고, 있는 그대로 문자열을 읽음

2장에서는 다음과 같이 어휘 분석을 하는 scan() 함수를 작성한다.
```cpp
auto main() -> void {
  string sourceCode = R""""(
    function main() {
      print("Hello, World!"); 
    } 
  )"""";
  auto tokenList = scan(sourceCode);
}
```

3장에서는 다음과 같이 구문 분석을 하는 parse() 함수를 작성한다.
```cpp
auto main() -> void {
  string sourceCode = R""""(
    function main() {
      print("Hello, World!"); 
    } 
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
}
```

4장에서는 다음과 같이 인터프리터를 만들어 본다.
```cpp
auto main() -> void {
  string sourceCode = R""""(
    function main() {
      print("Hello, World!"); 
    } 
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  interpret(syntaxTree);
}
```

5장에서는 3장에 이어서 바이트 코드를 생성하는 컴파일러를 만든다.
```cpp
auto main() -> void {
  string sourceCode = R""""(
    function main() {
      print("Hello, World!"); 
    } 
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  auto objectCode = generate(syntaxTree);
}
```

마지막 6장에서는 다음과 같이 바이트 코드를 실행하는 가상머신을 만들어 본다.
```cpp
auto main() -> void {
  string sourceCode = R""""(
    function main() {
      print("Hello, World!"); 
    } 
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  auto objectCode = generate(syntaxTree);
  execute(objectCode);
}
```

정리하면 2장에서는 scan() 함수를 구현하고, 3장에서는 parse() 함수를, 4장에서는 interpret() 함수를, 5장에서는 generate() 함수를, 6장에서는 execute() 함수를 구현한다. 예제 컴파일러는 표준 C++ 라이브러리 외에 다른 라이브러리나 도구를 사용하지 않는다.

마지막 부록 장에서는 어셈블리를 통해 실제로 코드가 CPU에서 어떻게 실행되는지 알아본다.