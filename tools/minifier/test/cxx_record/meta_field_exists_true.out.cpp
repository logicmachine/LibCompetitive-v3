template <typename T>
struct A {
private:
	template <typename U>
	static auto check(const U &x) -> decltype(x.a, int());
public:
	static const int value = sizeof(decltype(check(T())));
};
struct B {
	int a;
};
int main(){
	return A<B>::value;
}
