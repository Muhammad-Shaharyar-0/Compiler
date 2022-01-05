func Integer: numPrint (Integer: num, Integer: length)
{
	Integer: i, j, first;
	char : a;
	char : b;
	a := 'x';
	print("Enter Interger ");
	In>> i; /*Input for both char and integer*/
	print("Enter Character ");
	In>> b;
	println (i); /*output*/
	while i > 10 : /*Conditions and loops including bonus nested*/
	{
		first:= 0; 
		j := 10;
		while j < i:
		{
			first:= j + 1 -1 - 3; /*Arithmetic expression*/
			j := j + 1;
		}
		println (first);
		if j == 1:
		{
			print("one"); /*221*/
		}
		elif j==2: 
		{
			print("two"); 
		}
		else
		{
			if j>first:
			{
			    print(1);  /*string and character printing*/
			}
			print("others ");
			print('x');
		}

		/* this is a comment */
		i:= i - 1;
		/*This is a
		Multiline
		Comment*/
	}
	ret i;
}