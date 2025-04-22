# 3. 구문 분석

구문 분석은 프로그래밍 언어로 작성하 소스 코드의 구조를 분석하는 과정이다. 소스 코드의 구조는 문과 식으로 구성된다. 문은 복합문과 단일문으로 구분되며 for문이나 if문 등과 같이 다른 문을 포함할 수 있는 문을 복합문이라 하고, return문이나 continue문과 같이 다른 문을 포함할 수 없는 문을 단일문이라 한다.

따라서 문은 문을 포함할 수 있다. 또한 if문의 조건식이나 return문의 반환식과 같이 문은 식도 포함할 수 있다. 

식 1 + 2 * 3 을 예로 들어보자. 연산자의 우선순위에 따라 가장 먼저 계산되어야 할 것은 곱셈이다. 따라서 덧셈 연산자는 곱셈 식 2 * 3 을 포함한다.

정리하면 문은 문과 식을 포함하고 식은 식을 포함한다. 구문 분석은 소스 코드 구조의 문과 식을 분석하는 것이다.

> 문(statement): 어떤 작업을 수행하도록 지시하는 코드의 최소 단위. 실행 결과는 있지만, 특정 값을 반환하지는 않는다. 즉 "수행되는 명령" 이다.

> 식(expression): 하나의 값을 생성하는 코드의 단위. 계산이나 함수 호출 결과 등, 평가 후에 값을 반환한다. 즉 "평가되어 결과를 반환하는 것" 이다.

## 3.1 구문 트리와 노드

소스 코드의 구조를 분석해 문과 식의 포함관계를 트리로 표현한 것을 구문 트리라 한다. 트리는 노드들의 부모 자식 관계로 구성되므로 구문 분석을 하기 위해 가장 먼저 해야 할 것은 구문 트리를 구성할 노드들을 정의하는 것이다.

가장 먼저 정의할 노드는 구문 트리의 루트 노드이자 소스 코드의 선언 영역을 표현하는 노드다. 유랭은 선언 영역에 함수의 정의만 작성할 수 있다.

Node.h 파일을 만들고, 노드를 정의한다.

Statement와 Expression은 모든 문 노드와 식 노드의 부모 노드다. 이후 정의하는 노드들은 문이나 식에 따라 두 노드 중 하나를 상속받는다.
```cpp
struct Statement {};
struct Expression {};
```

함수 노드는 함수의 정의를 표현한다. 함수 이름, 매개 변수 이름 리스트, 실행할 문 리스트를 멤버로 가진다. (함수의 정의는 값을 반환하지 않으므로 문(Statement) 이다)

프로그램 노드는 함수 노드의 리스트를 멤버로 가진다.
```cpp
struct Function : Statement {
  string name;
  vector<string> parameters;
  vector<Statement> block;
};
struct Program {
  vector<Function*> functions;
};
```

다음 노드는 return문을 표현한다. 반환식을 멤버로 가진다.
```cpp
struct Return: Statement {
  Expression* expression;
}
```

다음 노드는 변수의 선언을 표현한다. 변수의 이름과 초기화식을 멤버로 가진다.
```cpp
struct Variable: Statement {
  string name;
  Expression* expression;
};
```

다음 노드는 for문을 표현한다. 변수의 선언, 조건식, 증감식, 실행할 문 리스트를 멤버로 가진다.
```cpp
struct For: Statement {
  Variable* variable;
  Expression* condition;
  Expression* expression;
  vector<Statement*> block;
}
```

다음 두 노드는 break문과 continue문을 표현한다. 멤버는 갖지 않는다.
```cpp
struct Break: Statement {};
struct Continue: Statement {};
```

다음 노드는 if문을 표현한다. 조건식 리스트, 각 조건식의 결과가 참일 때 실행할 문 리스트의 리스트, 거짓일 때 실행할 문 리스트를 멤버로 가진다.
```cpp
struct If: Statement {
  vector<Expression*> conditions;
  vector<vector<Statement*>> blocks;
  vector<Statement*> elseBlock;
}
```

다음 노드는 print문과 printLine문을 표현한다. 개행 여부와 콘솔에 출력할 식 리스트를 멤버로 가진다.
```cpp
struct Print: Statement {
  bool lineFeed = false;
  vector<Expression*> arguments;
}
```

다음 노드는 식의 문을 표현한다. 식은 항상 결과값을 남기는데 문에 속한 식의 결과값은 문의 목적에 따라 소비된다. 예를 들어 return 1 + 2; 라는 return 문에 속한 1 + 2의 결과값 3은 반환되어 소비된다. 또한 print(1 + 2); 라는 print문에 속한 식 1 + 2의 결과값 3도 출력되어 소비된다.

하지만 문에 포함되지 않은 식 1 + 2의 결과값 3은 그저 결과값일 뿐이다. 또한 함수를 호출한 후 반환값을 사용하지 않는 경우에도 마찬가지다. 이렇게 소비되지 않는 식의 결과값을 임의로 소비시키기 위해 식을 감싸는 문 노드가 필요하다.
```cpp
struct ExpressionStatement: Statement {
  Expression* expression;
};
```

다음 두 노드는 논리 연산자 or와 and를 표현한다. 논리 연산자는 이항 연산자이므로 왼쪽 식과 오른쪽 식을 멤버로 가진다. lhs는 왼쪽이라는 Left Hand Side의 약자이고, rhs는 오른쪽이라는 Right Hand Side의 약자이다.
```cpp
struct Or: Expression {
  Expression* lhs;
  Expression* rhs;
};
struct And: Expression {
  Expression* lhs;
  Expression* rhs;
}
```
논리 연산자 or와 and를 구분한 이유는 단락 평가 때문이다. false and () 는 왼쪽 식이 거짓이므로 오른쪽 식은 평가되지 않는다. 마찬가지로 true or () 는 왼쪽 식이 참이므로 오른쪽 식은 평가되지 않는다. 따라서 항상 양쪽 식을 모두 평가하는 관계 연산자와 산술 연산자와는 동작이 다르다.

다음 두 노드는 관계 연산자와 산술 연산자를 포함한다. 관계 연산자에는 ==, != 등이 있으며, 마찬가지로 산술 연산자에도 +, - 등 여러 종류가 있으므로 두 노드는 연산자의 종류를 멤버로 가진다. 또한 두 연산자 모두 이항 연산자이므로 왼쪽 피연산자 식과 오른쪽 피연산자 식을 멤버로 가진다.
```cpp
struct Relational: Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
};
struct Arithmetic: Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
};
```

다음 노드는 단항 연산자를 표현한다. 단항 연산자에는 절댓값을 구하는 +와 부호를 반전하는 -가 있으므로 연산자의 종류를 멤버로 갖고, 단항 연산자이므로 하나의 피연산자 식을 멤버로 갖는다. +연산자는 다른 언어들과 다르게 절댓값 연산자이다.
```cpp
struct Unary: Expression {
  Kind kind;
  Expression* sub;
};
```

다음 노드는 함수의 호출을 표현하는 식이다. 피연산자 식과 인자식 리스트를 멤버로 가진다.
```cpp
struct Call: Expression {
  Expression* sub;
  vector<Expression*> arguments;
};
```

다음 노드는 배열과 맵의 원소의 참조를 표현한다. 피연산자 식과 인덱스 식을 멤버로 가진다. array[0] 이나 map["property"] 같은 식을 표현한다.
```cpp
struct GetElement: Expression {
  Expression* sub;
  Expression* index;
};
```

다음 노드는 배열과 맵의 원소의 수정을 표현한다. 피연산자 식, 인덱스 식, 초기화식을 멤버로 가진다. array[0] = 3; 이나 map["property"] = 3; 과 같은 식을 표현한다.
```cpp
struct SetElement: Expression {
  Expression* sub;
  Expression* index;
  Expression* value;
};
```

다음 노드는 변수의 참조를 표현한다. 변수의 이름을 멤버로 가진다. Call 구조체와 GetElement, SetElement 구조체에서의 sub에 저장될 수 있다.
```cpp
struct GetVariable: Expression {
  string name;
};
```

다음 노드는 변수의 수정을 표현한다. 변수의 이름과 초기화식을 멤버로 가진다.
```cpp
struct SetVariable: Expression {
  string name;
  Expression* value;
};
```

다음 노드들은 순서대로 널 리터럴, 불리언 리터럴, 숫자 리터럴, 스트링 리터럴을 표현한다. 널 리터럴을 제외하고, 자신의 타입에 맞는 값을 멤버로 가진다.
```cpp
struct NullLiteral: Expression {};
struct BooleanLiteral: Expression {
  bool value = false;
};
struct NumberLiteral: Expression {
  double value = 0.0;
};
struct StringLiteral: Expression {
  string value;
};
```

다음 노드는 배열 리터럴을 표현한다. 원소식 리스트를 멤버로 가진다. [1, 2, 3]과 같은 리터럴 값을 표현한다.
```cpp
struct ArrayLiteral: Expression {
  vector<Expression*> values;
};
```

다음 노드는 맵 리터럴을 표현한다. 문자열과 원소식을 쌍으로 하는 맵을 멤버로 가진다. ["a": 1, "b": 2, "c": 3] 과 같은 리터럴 값을 표현한다.
```cpp
struct MapLiteral: Expression {
  map<string, Expression*> values;
};
```