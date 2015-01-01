struct X {
	static const bool value = true;
};
template <bool FLAG, typename T>
struct Y {
	static const int value = sizeof(T);
};
int main(){
	return Y<X::value, int>::value;
}
