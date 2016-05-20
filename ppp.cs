var int a = 1;
var bool bbb = true;


def print_string(string s) void {
	[s]_buildin_printstring_;
	return;
}

def print_int(int v) void {
	[v]_buildin_printint_;
	return;
}

def open(string filename) file {
	return [filename]_buildin_openfile_;
}

def read(string buff,int size,file file) int {
	return [buff,size,file]_buildin_readfile_;
}


def fib(int a) int {
	if (a == 0)
		return 0;
	if (a == 1)
		return 1;
	else {
		var int tmp = 0;
		$tmp = [a-1]fib + [a-2]fib;
		[tmp]print_int;
		["\n"]print_string;
		return tmp;
	}
}


def main() void {
	var file f = null;
	var string con = "null";
	
	[20]fib;

	$f = ["main.c"]open;
	if (f != null) {
		$a = [con,10000,f]read;
		[con + "\n"]print_string;
		[a]print_int;
	} else {
		["can't open"]print_string;
	}
	
	
	return;
}
