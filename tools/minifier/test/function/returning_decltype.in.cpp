int foo(){
	return 0;
}
auto bar() -> decltype(foo()) {
	return 0;
}
int main(){
	return bar();
}
