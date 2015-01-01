struct A {
	struct B {
		int b;
	};
	struct C {
		int c;
	};
	B b;
};
int main(){
	A a;
	a.b.b = 10;
	return a.b.b;
}
