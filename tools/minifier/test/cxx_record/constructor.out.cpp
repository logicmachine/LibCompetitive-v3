struct A {
	int a;
	int b;
	A(int a, int b)
		: a(a)
		, b(b)
	{ }
};
int main(){
	A a(1, 2);
	return a.a;
}
