struct A {
	int a;
	int b;
	A()
		: a(10)
		, b(20)
	{ }
	A(int a, int b)
		: a(a)
		, b(b)
	{ }
};
int main(){
	A a(1, 2);
	return a.a;
}
