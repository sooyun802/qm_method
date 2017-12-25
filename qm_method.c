#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct s_Item {
	char base2[10];
	int checked, numberOfOnes, numberOfTerms, terms[1024];
	struct s_Item *next;
}Item;
typedef struct s_group {
	int numberOfTerms;
	Item *minterm;
}Group;
typedef struct s_col {
	int num;
	Group index[10];
}Col;

int minimum_sop=10000000, sop_terms[1024];

void get_input(int *variableNm, int *mintermCnt, int values[], Col column[]);
void get_output(int variableNm, int mintermCnt, int primeImplicantCnt, Col column[], Item primeImplicants[]);
int find_primeImplicants(Col column[], int variableNm, Item primeImplicants[]);
void make_primeImplicantChart(int values[], int primeImplicantCnt, int mintermCnt, Item primeImplicants[], char primeImplicantChart[][mintermCnt]);
void QM_method(int primeImplicantCnt, int mintermCnt, char primeImplicantChart[][mintermCnt]);
int cross_out(int primeImplicantCnt, int mintermCnt, int sopNm, int x, char chart[][mintermCnt]);
void find_minimumSOP(int primeImplicantCnt, int mintermCnt, int sopNm, int x, char chart[][mintermCnt]);

int main() {
	int variableNm=0, mintermCnt=0, primeImplicantCnt, i, j, values[1024];
	Col column[10];
	Item primeImplicants[1024];

	for(i=0;i<10;i++) {
		column[i].num=i;
		for(j=0;j<10;j++) {
			column[i].index[j].numberOfTerms=0;
			column[i].index[j].minterm=NULL;
		}
	}
	
	get_input(&variableNm, &mintermCnt, values, column);
	
	primeImplicantCnt=find_primeImplicants(column, variableNm, primeImplicants);
	char primeImplicantChart[primeImplicantCnt][mintermCnt];

	make_primeImplicantChart(values, primeImplicantCnt, mintermCnt, primeImplicants, primeImplicantChart);
	QM_method(primeImplicantCnt, mintermCnt, primeImplicantChart);
	
	get_output(variableNm, mintermCnt, primeImplicantCnt, column, primeImplicants);

	return 0;
}

void get_input(int *variableNm, int *mintermCnt, int values[], Col column[]) {
	FILE *fin=fopen("input.txt", "r");
	char buf[255], tmp_base2[10];
	int i, j, tmp, numOfOnes;
	Item *m, *m_temp;

	fgets(buf, 255, (FILE *)fin);
	for(i=0;i<strlen(buf);i++) {
		if(isdigit(buf[i])) (*variableNm)=(*variableNm)*10+(buf[i]-'0');
	}
	fgets(buf, 255, (FILE *)fin);
	for(i=0;i<strlen(buf);i++) {
		if(isdigit(buf[i])) (*mintermCnt)=(*mintermCnt)*10+(buf[i]-'0');
	}
	for(i=0;i<(*mintermCnt);i++) {
		fgets(buf, 255, (FILE *)fin);
		tmp=0;
		for(j=0;j<strlen(buf);j++) {
			if(isdigit(buf[j])) tmp=tmp*10+(buf[j]-'0');
		}
		values[i]=tmp;
		numOfOnes=0;
		m=(Item *)malloc(sizeof(Item));
		memset(tmp_base2, 0, 10);
		m->checked=0;
		m->terms[0]=tmp;
		m->numberOfTerms=1;
		for(j=0;tmp>0;j++) {
			tmp_base2[j]=tmp%2;
			if(tmp_base2[j]==1) numOfOnes++;
			tmp=tmp/2;
		}
		for(j=(*variableNm)-1;j>=0;j--) {
			m->base2[(*variableNm)-1-j]=tmp_base2[j]+'0';
		}
		m->numberOfOnes=numOfOnes;
		m->next=NULL;
		m_temp=column[0].index[m->numberOfOnes].minterm;
		if(m_temp==NULL) column[0].index[m->numberOfOnes].minterm=m;
		else {
			while(m_temp->next!=NULL) {
				m_temp=m_temp->next;
			}
			m_temp->next=m;
		}
		column[0].index[m->numberOfOnes].numberOfTerms++;
	}
	fclose(fin);
}

void get_output(int variableNm, int mintermCnt, int primeImplicantCnt, Col column[], Item primeImplicants[]) {
	FILE *fout=fopen("output.txt", "w");
	int i, j;
	Item *m_temp1, *m_temp2;

	fprintf(fout, "Number of final terms : %d\n", primeImplicantCnt);
	fprintf(fout, "Final terms\n");
	for(i=0;i<primeImplicantCnt;i++) {
		fprintf(fout, "(");
		for(j=0;j<primeImplicants[i].numberOfTerms;j++) {
			if(j==primeImplicants[i].numberOfTerms-1) fprintf(fout, "%d)\n", primeImplicants[i].terms[j]);
			else fprintf(fout, "%d, ", primeImplicants[i].terms[j]);
		}
	}
	fprintf(fout, "Minimum SOP : ");
	for(i=0;i<minimum_sop;i++) {
		for(j=0;j<variableNm;j++) {
			if(primeImplicants[sop_terms[i]].base2[j]=='X') continue;
			fprintf(fout, "%c", j+'a');
			if(primeImplicants[sop_terms[i]].base2[j]=='0') fprintf(fout, "\'");
		}
		if(i==minimum_sop-1) fprintf(fout, "\n");
		else fprintf(fout, " + ");
	}

	for(i=0;i<variableNm;i++) {
		for(j=0;j<=variableNm;j++) {
			m_temp1=column[i].index[j].minterm;
			while(m_temp1!=NULL) {
				m_temp2=m_temp1->next;
				free(m_temp1);
				m_temp1=m_temp2;
			}
		}
	}
	if(mintermCnt==0) fprintf(fout, "0\n");
	else if(minimum_sop==0) fprintf(fout, "1\n");

	fclose(fout);
}

int find_primeImplicants(Col column[], int variableNm, Item primeImplicants[]) {
	int i, j, k, l, difference, flag, colNm, primeImplicantCnt=0;
	Item *m, *m_temp, *m_temp1, *m_temp2;

	for(colNm=0;colNm<variableNm;colNm++) {
		for(i=0;i<variableNm;i++) {
			m_temp1=column[colNm].index[i].minterm;
			m_temp2=column[colNm].index[i+1].minterm;
			if(m_temp1==NULL || m_temp2==NULL) continue;
			for(j=0;j<column[colNm].index[i].numberOfTerms;j++) {
				for(k=0;k<column[colNm].index[i+1].numberOfTerms;k++) {
					difference=0;
					for(l=0;l<variableNm;l++) {
						if(m_temp1->base2[l]!=m_temp2->base2[l]) difference++;
					}
					if(difference==1) { //the difference between two Items is 1-bit
						m_temp1->checked=1;
						m_temp2->checked=1;
						m=(Item *)malloc(sizeof(Item));
						memset(m->base2, 0, 10);
						for(l=0;l<variableNm;l++) {
							if(m_temp1->base2[l]!=m_temp2->base2[l]) m->base2[l]='X';
							else m->base2[l]=m_temp1->base2[l];
						}
						m->numberOfOnes=m_temp1->numberOfOnes;
						m->checked=0;
						m->next=NULL;
						for(l=0;l<m_temp1->numberOfTerms;l++) {
							m->terms[l]=m_temp1->terms[l];
						}
						for(l=0;l<m_temp2->numberOfTerms;l++) {
							m->terms[l+m_temp1->numberOfTerms]=m_temp2->terms[l];
						}
						m->numberOfTerms=m_temp1->numberOfTerms+m_temp2->numberOfTerms;
						m_temp=column[colNm+1].index[m->numberOfOnes].minterm;
						flag=0;
						if(m_temp==NULL) column[colNm+1].index[m->numberOfOnes].minterm=m; //current Item is first added to group
						else {
							while(m_temp) { //check for repetition
								if(strcmp(m_temp->base2, m->base2)==0) flag=1;
								if(m_temp->next==NULL) break;
								m_temp=m_temp->next;
							}
							if(flag!=1) m_temp->next=m; //add only if current Item has not been added before
							else free(m);
						}
						if(flag!=1) column[colNm+1].index[m->numberOfOnes].numberOfTerms++;
					}
					m_temp2=m_temp2->next; //get next Item of group[i+1]
				}
				m_temp1=m_temp1->next; //get next Item of group[i]
				m_temp2=column[colNm].index[i+1].minterm;
			}
		}

		for(i=0;i<=variableNm;i++) {
			m_temp=column[colNm].index[i].minterm;
			while(m_temp!=NULL) {
				if(m_temp->checked==0) { //if Item has not been checked -> is prime implicant
					strcpy(primeImplicants[primeImplicantCnt].base2, m_temp->base2);
					primeImplicants[primeImplicantCnt].checked=m_temp->checked;
					primeImplicants[primeImplicantCnt].numberOfOnes=m_temp->numberOfOnes;
					primeImplicants[primeImplicantCnt].numberOfTerms=m_temp->numberOfTerms;
					for(j=0;j<m_temp->numberOfTerms;j++) {
						primeImplicants[primeImplicantCnt].terms[j]=m_temp->terms[j];
					}
					primeImplicants[primeImplicantCnt++].next=NULL;
				}
				m_temp=m_temp->next;
			}
		}
	}

	return primeImplicantCnt;
}

void make_primeImplicantChart(int values[], int primeImplicantCnt, int mintermCnt, Item primeImplicants[], char primeImplicantChart[][mintermCnt]) {
	int i, j, k;

	for(i=0;i<primeImplicantCnt;i++) {
		for(j=0;j<mintermCnt;j++) {
			primeImplicantChart[i][j]='_';
		}
	}
	for(i=0;i<primeImplicantCnt;i++) {
		for(j=0;j<primeImplicants[i].numberOfTerms;j++) {
			for(k=0;k<mintermCnt;k++) {
				if(primeImplicants[i].terms[j]==values[k]) { //minterm is prime implicant
					primeImplicantChart[i][k]='X';
					break;
				}
			}
		}
	}
}

void QM_method(int primeImplicantCnt, int mintermCnt, char primeImplicantChart[][mintermCnt]) {
	int i, j, flag, tmp, sopNm=0;

	for(i=0;i<mintermCnt;i++) {
		flag=0;
		for(j=0;j<primeImplicantCnt;j++) {
			if(primeImplicantChart[j][i]=='X') {
				tmp=j;
				flag++;
			}
		}
		if(flag==1) { //is essential prime implicant
			primeImplicantChart[tmp][i]='O';
		}
	}
	for(i=0;i<primeImplicantCnt;i++) {
		for(j=0;j<mintermCnt;j++) {
			if(primeImplicantChart[i][j]=='O') {
				sopNm=cross_out(primeImplicantCnt, mintermCnt, sopNm, i, primeImplicantChart); //cross out essential prime implicants and its related items from prime implicant chart
			}
		}
	}
	for(i=0;i<primeImplicantCnt;i++) {
		for(j=0;j<mintermCnt;j++) {
			if(primeImplicantChart[i][j]=='X') find_minimumSOP(primeImplicantCnt, mintermCnt, sopNm, i, primeImplicantChart); //remove left items on prime implicant chart
		}
	}

	if(minimum_sop==10000000) {
		minimum_sop=sopNm;
		tmp=0;
		for(i=0;i<primeImplicantCnt;i++) {
			if(primeImplicantChart[i][0]=='-') sop_terms[tmp++]=i;
		}
	}
}

int cross_out(int primeImplicantCnt, int mintermCnt, int sopNm, int x, char chart[][mintermCnt]) {
	int i, j;

	for(i=0;i<mintermCnt;i++) {
		if(chart[x][i]=='X') {
			for(j=0;j<primeImplicantCnt;j++) {
				if(chart[j][i]!='-')chart[j][i]='|';
			}
		}
		chart[x][i]='-';
	}

	return ++sopNm;
}

void find_minimumSOP(int primeImplicantCnt, int mintermCnt, int sopNm, int x, char chart[][mintermCnt]) {
	int tmp=0, i, j, cnt=0;
	char copy[primeImplicantCnt][mintermCnt];

	for(i=0;i<primeImplicantCnt;i++) { //make copy of current prime implicant chart
		for(j=0;j<mintermCnt;j++) {
			copy[i][j]=chart[i][j];
		}
	}
	sopNm=cross_out(primeImplicantCnt, mintermCnt, sopNm, x, copy);

	for(i=0;i<primeImplicantCnt;i++) {
		for(j=0;j<mintermCnt;j++) {
			if(copy[i][j]=='X') cnt++;
		}
	}
	if(cnt==0) { //no item left on prime implicant chart
		if(minimum_sop>=sopNm) { //current SOP has smaller numer of products
			minimum_sop=sopNm;
			for(i=0;i<primeImplicantCnt;i++) {
				if(copy[i][0]=='-') sop_terms[tmp++]=i; //save products of current SOP
			}
		}
		return;
	}

	for(i=0;i<primeImplicantCnt;i++) {
		for(j=0;j<mintermCnt;j++) {
			if(copy[i][j]=='X') find_minimumSOP(primeImplicantCnt, mintermCnt, sopNm, i, copy); //repeat process
		}
	}
}
