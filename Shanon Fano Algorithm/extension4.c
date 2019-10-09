//============================================================================
// Name        : CCD.cpp
// Author      : mdmh
// Copyright   : Your copyright notice
// Description : validate the Shanon Theorem using a source with extension 4
//============================================================================

#include "stdio.h"
using namespace std;

int main() {


	double probabilidad [16]=
	{
			0.2401,

			0.1029,
			0.1029,
			0.1029,
			0.1029,

			0.0441,
			0.0441,
			0.0441,
			0.0441,
			0.0441,
			0.0441,

			0.0189,
			0.0189,
			0.0189,
			0.0189,

			0.0081
	} ;

	double cantidad[16]={
			3,3,3,3,3,3,3,
			4,
			5,
			6,
			8,8,8,
			9,
			10,10,

	};

	double Ln=0;

	double Hs = 0.88;


	for (int i =0; i<16;i++)
	{
		printf ("(%f * %f) = %f\n",probabilidad[i],cantidad[i],probabilidad[i]*cantidad[i]);
		Ln+=probabilidad[i]*cantidad[i];
	}

	printf ("Ln=%f\n",Ln);
	printf ("H(s)<=Ln/n<=H(s)+1/n\n");
	printf ("%f<=%f/%i<=%f+1/%i\n",Hs,Ln,4,Hs,4);
	Ln/=4;
	printf ("%f<=%f<=%f+%f\n",Hs,Ln,Hs,1.0/4.0);

	printf ("%f<=%f<=%f\n",Hs,Ln,Hs+(1.0/4.0));


	int r = Hs<=Ln && Ln<=(Hs+(1/4));
	printf ("Resultado:%i",r);

	return 0;
}






























