struct A {
};
struct B {
};
template <class T, class U>
struct C {
	typedef U type;
};
auto func()
	-> C<A, B>::type
{
	return B();
}
int main(){
	func();
	return 0;
}
