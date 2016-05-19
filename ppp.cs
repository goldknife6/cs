var int a = 1;
var bool bbb = true;

def fib(int a) int {
	if (a == 0)
		return 0;
	if (a == 1)
		return 1;
	else
		return [a-1]fib + [a-2]fib;
}


def print_string(string s) void {
	[s]_buildin_printstring_;
	return;
}

def print_int(int v) void {
	[v]_buildin_printint_;
	return;
}

def main() void {
	$a = [10]fib;
	/*
	while(bbb)
	{
		var int a = 0;
		//$a = [10]fib + [10]fib + [10]fib;
		$a = a * 10;
		if (a != 0)
			break;
	}*/
	for (;a < 1111000; $a = a + 1) {
		[a]print_int;
		["aaaa"+" bbbbbb"+"a\n"]print_string;

	}
	return;
}
