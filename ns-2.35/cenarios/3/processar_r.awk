BEGIN{
i=0;
}

{
   action=$1;
   cl1=$3;
   cl2=$5;
   if( (action=="r") ){
	c1[i]=cl1;
	c2[i]=cl2;
	i++;
   }
}

END {
   #for(j=0;j<i;j++){
   #	printf("%f %d\n", c1[j], c2[j]);
   #}
   #printf("%d\n",length(c1));
   tempo = 5;
   inicio=0;
   fim=1;
   for(k=0;k<tempo;k++) {
	cont=0;
   	for(j=0;j<length(c1);j++) {
		if ((c1[j]>=inicio)&&(c1[j]<fim)) {
			cont++;
		}
   	}
	inicio++;
	fim++;
	c3[k] = inicio;
	c4[k] = cont;
   }
   for(j=0;j<length(c3);j++){
        printf("%d %d\n", c3[j], c4[j]);
   }

}
