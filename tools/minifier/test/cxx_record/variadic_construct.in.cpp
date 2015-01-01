struct A {
	int a;
	int b;
	A(int a, int b)
		: a(a)
		, b(b)
	{ }
};
template <typename Base>
struct B : public Base {
	template <typename... Args>
	B(Args&&... args)
		: Base(args...)
	{ }
};
int main(){
	B<A> b(10, 11);
	return b.a;
}
