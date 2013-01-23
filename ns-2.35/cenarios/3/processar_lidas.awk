BEGIN{
i=0;
}

{
   action=$1;
   cl1=$3;
   cl2=$31;
   if( (action=="r") ){
	c1[i]=cl1;
	c2[i]=cl2;
	i++;
   }
}

END {
   cont=0;
   asort(c2);
   for(j=0;j<=i;j++){
   	if ( ( c2[j]!=0 ) ) {
		printf("%d\n",c2[j]);
		cont++;
	}
   }
   #printf("%d\n",length(c1));
   # printf("Total: %d\n",cont);

}
