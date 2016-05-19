var int a = 0;
def fib(int a) int {
	if (a == 0)
		return 0;
	if (a == 1)
		return 1;
	else
		return [a-1]fib + [a-2]fib;
}

def main() void {
	$a = [23]fib;
	while(true)
	{
		if (a == 28657)
			break;
	}
	return;
}
