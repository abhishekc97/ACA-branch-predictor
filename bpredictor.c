/* Implementation for Level 1 and Level 2 cache, which is a part of memory hierarchy
 * Uses LRU, FIFO and Optimal Policies and Inclusive and Non-Inclusive property
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* 
 *Defining a struct for a cachememoryblock
 */

typedef struct CACHEMEMORYBLOCK
{
	int VALID;
	int DIRTY;
	int POLICY_COUNTER;
	unsigned long long TAG;
	
}CACHE;


/* 
 * Initialize required variables
 */

int Level_1_Read = 0;
int Level_2_Read = 0;
int Level_1_Write = 0;
int Level_2_Write = 0;

int Level_1_Cache_Size;
int Level_2_Cache_Size;

int Level_1_Block_Size;
int Level_2_Block_Size;

int Level_1_Assoc;
int Level_2_Assoc;

int Level_1_Read_Hit = 0;
int Level_2_Read_Hit = 0;
int Level_1_Write_Hit = 0;
int Level_2_Write_Hit = 0;


int Level_1_Read_Miss = 0;
int Level_2_Read_Miss = 0;
int Level_1_Write_Miss = 0;
int Level_2_Write_Miss = 0;

int Level_1_Write_Back = 0;
int Level_2_Write_Back = 0;

char Level_1_Policy[10];
char Level_2_Policy[10];

int Level_1_Policy_counter = 0;
int Level_2_Policy_counter = 0;

int Level_1_Tag_Width = 0;
int Level_2_Tag_Width = 0;

int Level_1_Index_Width = 0;
int Level_2_Index_Width = 0;

int Level_1_Offset_Width = 0;
int Level_2_Offset_Width = 0;

float Level_1_Miss_Rate;
float Level_2_Miss_Rate;

unsigned long long Address, Level_1_Tag, Level_2_Tag;
unsigned long Level_1_Index_Decimal = 0, Level_2_Index_Decimal = 0;
unsigned long Level_1_Offset_Decimal = 0, Level_2_Offset_Decimal;

int Memory_Traffic;

/* 
 * Function declarations
 */

void LEVEL1ReadCache(CACHE LEVEL1[][32], CACHE LEVEL2[][32]);
void LEVEL1WriteCache(CACHE LEVEL1[][32], CACHE LEVEL2[][32]);
int LEVEL1CachePolicy(CACHE LEVEL1[][32]);

void LEVEL2ReadCache(CACHE LEVEL2[][32]);
void LEVEL2WriteCache(CACHE LEVEL1[][32], CACHE LEVEL2[][32], int);
int LEVEL2CachePolicy(CACHE LEVEL2[][32]);

int LEVEL2WriteCachePolicy(CACHE LEVEL2[][32], int);
int LEVEL2WriteLRU(CACHE LEVEL2[][32], int);

int LRUPolicy(CACHE LEVEL1[][32]);
int LRUPolicy2(CACHE LEVEL2[][32]);


/* 
 * Main function to run, handles cmd line arguments and caclulates hits , rates, makes L! and L2 cache and print output in command line screen
 */
int main(int argc, char *argv[]){
	int i, j, a, b;
	a = atoi(argv[6]);
	b = atoi(argv[7]);
	int level_1_block_size = atoi(argv[1]);
	int level_1_cache_size = atoi(argv[2]);
	int level_1_assoc = atoi(argv[3]);
	CACHE LEVEL1[1000][32];
	Level_1_Assoc = level_1_assoc;
	Level_1_Block_Size = level_1_block_size;
	Level_1_Cache_Size = level_1_cache_size;
	Level_1_Index_Width = (int)(log(level_1_cache_size/(level_1_assoc*level_1_block_size))/log(2));
	Level_1_Offset_Width = (int)(log(level_1_block_size)/log(2));
	strcpy(Level_1_Policy, argv[6]);
	Level_1_Tag_Width = 64- Level_1_Index_Width - Level_1_Offset_Width;

	for(i=0;i<level_1_cache_size/(level_1_assoc*level_1_block_size);i++){
		for(j=0;j<level_1_assoc;j++){
			LEVEL1[i][j].VALID = 0;
			LEVEL1[i][j].DIRTY = 0;
			LEVEL1[i][j].TAG = 0;
			LEVEL1[i][j].POLICY_COUNTER = 0;
		}
	}

	int level_2_block_size = atoi(argv[1]);
	int level_2_cache_size = atoi(argv[4]);
	int level_2_assoc = atoi(argv[5]);
	CACHE LEVEL2[2000][32];
	if(level_2_assoc == 0 || level_2_block_size == 0){
		Level_2_Cache_Size = 0;
	}
	else{
		Level_2_Cache_Size = level_2_cache_size;
		Level_2_Block_Size = level_2_block_size;
		Level_2_Assoc = level_2_assoc;
		Level_2_Index_Width = (int)(log(level_2_cache_size/(level_2_assoc*level_2_block_size))/log(2));
		Level_2_Offset_Width = (int)(log(level_2_block_size)/log(2));
		strcpy(Level_2_Policy, argv[6]);

		for(i = 0; i< level_2_cache_size/(level_2_assoc*level_2_block_size); i++){
			for(j=0; j<level_2_assoc; j++){
				LEVEL2[i][j].VALID = 0;
				LEVEL2[i][j].DIRTY = 0;
				LEVEL2[i][j].TAG = 0;
				LEVEL2[i][j].POLICY_COUNTER = 0;
			}
		}
	}

	char read_write = '0';

	FILE *fp;
	fp = fopen(argv[8], "r");
	fseek(fp,0,SEEK_SET);
	if(fp == NULL){
		printf("The given file is possibly empty\n");
		exit(1);
	}

	while(1){
		if(feof(fp))
		{
			break;
		}
		fscanf(fp,"%c %llx \n", &read_write, &Address);
		Level_1_Tag = Address>>(Level_1_Index_Width+Level_1_Offset_Width);
		Level_1_Index_Decimal = (Address << Level_1_Tag_Width) >> (Level_1_Tag_Width + Level_1_Offset_Width);
		Level_1_Offset_Decimal = (Address << (Level_1_Tag_Width + Level_1_Index_Width)) >> (Level_1_Tag_Width+Level_1_Index_Width);

	
	if(Level_2_Cache_Size != 0){
		Level_2_Tag_Width = 64 - Level_2_Index_Width - Level_2_Offset_Width;
		Level_2_Tag = Address>>(Level_2_Index_Width+Level_2_Offset_Width);
		Level_2_Index_Decimal = (Address << Level_2_Tag_Width) >> (Level_2_Tag_Width + Level_2_Offset_Width);
		Level_2_Offset_Decimal = (Address << (Level_2_Tag_Width + Level_2_Index_Width)) >> (Level_2_Tag_Width+Level_2_Index_Width);
	}
	if(read_write == 'r')
		LEVEL1ReadCache(LEVEL1,LEVEL2);
	else if(read_write == 'w')
		LEVEL1WriteCache(LEVEL1,LEVEL2);
	else printf("Error occured\n");
	}
	Level_1_Miss_Rate = (float)(((float)Level_1_Read_Miss+(float)Level_1_Write_Miss)/((float)Level_1_Read+(float)Level_1_Write));
	if(Level_2_Cache_Size == 0)
		Level_2_Miss_Rate = 0;
	else
		Level_2_Miss_Rate = (float)(((float)Level_2_Read_Miss+(float)Level_2_Write_Miss)/((float)Level_2_Read+(float)Level_2_Write));
	if(Level_2_Cache_Size == 0)
		Memory_Traffic = Level_1_Read_Miss + Level_1_Write_Miss + Level_1_Write_Back;
	else
		Memory_Traffic = Level_2_Read_Miss + Level_2_Write_Miss + Level_2_Write_Back;


printf("===== Simulator configuration =====\n");
printf("BLOCKSIZE:             %s\n",argv[1]);
printf("L1_SIZE:               %s\n",argv[2]);
printf("L1_ASSOC:              %s\n",argv[3]);
printf("L2_SIZE:               %s\n",argv[4]);
printf("L2_ASSOC:              %s\n",argv[5]);
if(a==0)
	printf("REPLACEMENT POLICY:    LRU\n");
else if(a==1)
	printf("REPLACEMENT POLICY:    FIFO\n");
else if(a==2)
	printf("REPLACEMENT POLICY:    Optimal\n");
	else printf("REPLACEMENT POLICY:    ---\n");

if(b==0)
	printf("INCLUSION PROPERTY:    non-inclusive\n");
else if(b==1)
	printf("INCLUSION PROPERTY:    inclusive\n");
else printf("INCLUSION PROPERTY:    ---\n");

printf("trace_file:            %s\n",argv[8]);
printf("===== Simulation results (raw) =====\n");
printf("a. number of L1 reads:        %d\n",Level_1_Read);
printf("b. number of L1 read misses:  %d\n",Level_1_Read_Miss);
printf("c. number of L1 writes:       %d\n",Level_1_Write);
printf("d. number of L1 write misses: %d\n",Level_1_Write_Miss);
printf("e. L1 miss rate:              %f\n",Level_1_Miss_Rate);
printf("f. number of L1 writebacks:   %d\n",Level_1_Write_Back);
printf("g. number of L2 reads:        %d\n",Level_2_Read);
printf("h. number of L2 read misses:  %d\n",Level_2_Read_Miss);
printf("i. number of L2 writes:       %d\n",Level_2_Write);
printf("j. number of L2 write misses: %d\n",Level_2_Write_Miss);
printf("k. L2 miss rate:              %f\n",Level_2_Miss_Rate);
printf("l. number of L2 writebacks:   %d\n",Level_2_Write_Back);
printf("m. total memory traffic:      %d",Memory_Traffic);

fclose(fp);
return 0;
}


/* 
 * read cache for level1 calculates reads,  policycounts, read miss
 */

void LEVEL1ReadCache(CACHE LEVEL1[][32], CACHE LEVEL2[][32])
{
	int i, countera = 0, counterb = 0, standing = 0;
	Level_1_Read += 1;
	Level_1_Policy_counter += 1;
	if(strcasecmp(Level_1_Policy,"0") == 0 || strcasecmp(Level_1_Policy,"2") == 0 || strcasecmp(Level_1_Policy,"3") == 0){
		for(i=0; i<Level_1_Assoc; i++){
			if(LEVEL1[Level_1_Index_Decimal][i].VALID == 1 && LEVEL1[Level_1_Index_Decimal][i].TAG == Level_1_Tag){
				Level_1_Read_Hit += 1;
				countera = 1;
				LEVEL1[Level_1_Index_Decimal][i].POLICY_COUNTER = Level_1_Policy_counter;
				break;
			}
		}
	if(countera == 0){
		Level_1_Read_Miss +=1;
		for(i=0; i<Level_1_Assoc; i++){
			if(LEVEL1[Level_1_Index_Decimal][i].VALID == 0){
				standing = i;
				break;
			}
			if(LEVEL1[Level_1_Index_Decimal][i].VALID == 1){
				counterb +=1;
			}
		}
		if(counterb == Level_1_Assoc){
			standing = LEVEL1CachePolicy(LEVEL1);
		}
		if(LEVEL1[Level_1_Index_Decimal][standing].DIRTY ==1 && LEVEL1[Level_1_Index_Decimal][standing].VALID == 1){
			Level_1_Write_Back += 1;
			if(Level_2_Cache_Size != 0)
				LEVEL2WriteCache(LEVEL1, LEVEL2, standing);
		}
		if(Level_2_Cache_Size!=0)
			LEVEL2ReadCache(LEVEL2);
			LEVEL1[Level_1_Index_Decimal][standing].TAG = Level_1_Tag;
			LEVEL1[Level_1_Index_Decimal][standing].POLICY_COUNTER = Level_1_Policy_counter;
			LEVEL1[Level_1_Index_Decimal][standing].DIRTY = 0;
			LEVEL1[Level_1_Index_Decimal][standing].VALID = 1;
	}
	}
	else if(strcasecmp(Level_1_Policy,"1") == 0){
			for(i=0; i<Level_1_Assoc; i++){
			if(LEVEL1[Level_1_Index_Decimal][i].VALID == 1 && LEVEL1[Level_1_Index_Decimal][i].TAG == Level_1_Tag){
				Level_1_Read_Hit += 1;
				countera = 1;
				break;
			}
		}
		if(countera == 0){
			Level_1_Read_Miss +=1;
			for(i=0; i<Level_1_Assoc; i++){
				if(LEVEL1[Level_1_Index_Decimal][i].VALID == 0){
					standing = i;
					break;
				}
				if(LEVEL1[Level_1_Index_Decimal][i].VALID == 1){
					counterb +=1;
				}
			}
			if(counterb == Level_1_Assoc){
				standing = LEVEL1CachePolicy(LEVEL1);
			}
			if(LEVEL1[Level_1_Index_Decimal][standing].DIRTY ==1 && LEVEL1[Level_1_Index_Decimal][standing].VALID == 1){
				Level_1_Write_Back += 1;
				if(Level_2_Cache_Size != 0)
					LEVEL2WriteCache(LEVEL1, LEVEL2, standing);
			}
			if(Level_2_Cache_Size!=0)
				LEVEL2ReadCache(LEVEL2);
				LEVEL1[Level_1_Index_Decimal][standing].TAG = Level_1_Tag;
				LEVEL1[Level_1_Index_Decimal][standing].POLICY_COUNTER = Level_1_Policy_counter;
				LEVEL1[Level_1_Index_Decimal][standing].DIRTY = 0;
				LEVEL1[Level_1_Index_Decimal][standing].VALID = 1;
		}
	}

}

int LEVEL1CachePolicy(CACHE LEVEL1[][32])
{
	int standing;
		if(strcasecmp(Level_1_Policy,"0") == 0 || strcasecmp(Level_1_Policy,"2") == 0 || strcasecmp(Level_1_Policy,"3") == 0)
			standing = LRUPolicy(LEVEL1);
		else if (strcasecmp(Level_1_Policy,"1") == 0)
			standing = LRUPolicy(LEVEL1);
		
		return standing;	
}

int LRUPolicy(CACHE LEVEL1[][32])
{
	int i, LRU = 0, WAY_LRU = 0;
	LRU = LEVEL1[Level_1_Index_Decimal][0].POLICY_COUNTER;
	for (i = 0; i < Level_1_Assoc; i++){
		if(LEVEL1[Level_1_Index_Decimal][i].POLICY_COUNTER < LRU){
			LRU = LEVEL1[Level_1_Index_Decimal][i].POLICY_COUNTER;
			WAY_LRU = i;
		}
	}
	return WAY_LRU;
}

void LEVEL1WriteCache(CACHE LEVEL1[][32], CACHE LEVEL2[][32])
{
	int i, countera = 0, counterb = 0;
	int standing;
	Level_1_Write += 1;
	Level_1_Policy_counter += 1;

	if(strcasecmp(Level_1_Policy,"0") == 0 || strcasecmp(Level_1_Policy,"2") == 0 || strcasecmp(Level_1_Policy,"3") == 0){
		for(i=0; i < Level_1_Assoc; i++){
			if(LEVEL1[Level_1_Index_Decimal][i].VALID == 1 && (Level_1_Tag == LEVEL1[Level_1_Index_Decimal][i].TAG)){
				Level_1_Write_Hit += 1;
				countera = 1;
				LEVEL1[Level_1_Index_Decimal][i].POLICY_COUNTER = Level_1_Policy_counter;
				LEVEL1[Level_1_Index_Decimal][i].DIRTY = 1;
				break;
			}
		}
		if(countera == 0){
			Level_1_Write_Miss+=1;
			for(i=0; i < Level_1_Assoc; i++){
				if(LEVEL1[Level_1_Index_Decimal][i].VALID == 0){
					standing = i;
					break;
				}
				else
					counterb += 1;
			}
			if(counterb==Level_1_Assoc){
				standing = LEVEL1CachePolicy(LEVEL1);
			}
			if(LEVEL1[Level_1_Index_Decimal][standing].DIRTY == 1 && LEVEL1[Level_1_Index_Decimal][standing].VALID == 1){
				Level_1_Write_Back += 1;
				if(Level_2_Cache_Size != 0)
					LEVEL2WriteCache(LEVEL1, LEVEL2, standing);
			}
			if(Level_2_Cache_Size != 0)
				LEVEL2ReadCache(LEVEL2);
			LEVEL1[Level_1_Index_Decimal][standing].TAG = Level_1_Tag;
			LEVEL1[Level_1_Index_Decimal][standing].POLICY_COUNTER = Level_1_Policy_counter;
			LEVEL1[Level_1_Index_Decimal][standing].DIRTY = 1;
			LEVEL1[Level_1_Index_Decimal][standing].VALID = 1;

		}

	}
	else if(strcasecmp(Level_1_Policy,"1") == 0){
		for(i=0; i < Level_1_Assoc; i++){
			if(LEVEL1[Level_1_Index_Decimal][i].VALID == 1 && (Level_1_Tag == LEVEL1[Level_1_Index_Decimal][i].TAG)){
				Level_1_Write_Hit += 1;
				countera = 1;
				LEVEL1[Level_1_Index_Decimal][i].POLICY_COUNTER = Level_1_Policy_counter;
				LEVEL1[Level_1_Index_Decimal][i].DIRTY = 1;
				break;
			}
		}
		if(countera == 0){
			Level_1_Write_Miss+=1;
			for(i=0; i < Level_1_Assoc; i++){
				if(LEVEL1[Level_1_Index_Decimal][i].VALID == 0){
					standing = i;
					break;
				}
				else
					counterb += 1;
			}
			if(counterb==Level_1_Assoc){
				standing = LEVEL1CachePolicy(LEVEL1);
			}
			if(LEVEL1[Level_1_Index_Decimal][standing].DIRTY == 1 && LEVEL1[Level_1_Index_Decimal][standing].VALID == 1){
				Level_1_Write_Back += 1;
				if(Level_2_Cache_Size != 0)
					LEVEL2WriteCache(LEVEL1, LEVEL2, standing);
			}
			if(Level_2_Cache_Size != 0)
				LEVEL2ReadCache(LEVEL2);
			LEVEL1[Level_1_Index_Decimal][standing].TAG = Level_1_Tag;
			LEVEL1[Level_1_Index_Decimal][standing].POLICY_COUNTER = Level_1_Policy_counter;
			LEVEL1[Level_1_Index_Decimal][standing].DIRTY = 1;
			LEVEL1[Level_1_Index_Decimal][standing].VALID = 1;

		}

	}
}

void LEVEL2ReadCache(CACHE LEVEL2[][32])
{
	int i, countera = 0, counterb = 0;
	int standing = 0;

	Level_2_Read += 1;
	Level_2_Policy_counter += 1;

	if(strcasecmp(Level_2_Policy,"0") == 0 || strcasecmp(Level_2_Policy,"2") == 0 || strcasecmp(Level_2_Policy,"3") == 0){
		for(i=0; i<Level_2_Assoc; i++){
			if(LEVEL2[Level_2_Index_Decimal][i].VALID == 1 && LEVEL2[Level_2_Index_Decimal][i].TAG == Level_2_Tag){
				Level_2_Read_Hit += 1;
				countera = 1;
				LEVEL2[Level_2_Index_Decimal][i].POLICY_COUNTER = Level_2_Policy_counter;
				break;
			}
		}
	if(countera == 0){
		Level_2_Read_Miss +=1;
		for(i=0; i<Level_2_Assoc; i++){
			if(LEVEL2[Level_2_Index_Decimal][i].VALID == 0){
				standing = i;
				break;
			}
			if(LEVEL2[Level_2_Index_Decimal][i].VALID == 1){
				counterb +=1;
			}
		}
		if(counterb == Level_2_Assoc){
			standing = LEVEL2CachePolicy(LEVEL2);
		}
		if(LEVEL2[Level_2_Index_Decimal][standing].DIRTY ==1 && LEVEL2[Level_2_Index_Decimal][standing].VALID == 1){
			Level_2_Write_Back += 1;
		}
		LEVEL2[Level_2_Index_Decimal][standing].TAG = Level_2_Tag;
		LEVEL2[Level_2_Index_Decimal][standing].POLICY_COUNTER = Level_2_Policy_counter;
		LEVEL2[Level_2_Index_Decimal][standing].DIRTY = 0;
		LEVEL2[Level_2_Index_Decimal][standing].VALID = 1;
	}
	}
	else if(strcasecmp(Level_2_Policy,"1") == 0){
			for(i=0; i<Level_2_Assoc; i++){
			if(LEVEL2[Level_2_Index_Decimal][i].VALID == 1 && LEVEL2[Level_2_Index_Decimal][i].TAG == Level_2_Tag){
				Level_2_Read_Hit += 1;
				countera = 1;
				break;
			}
		}
		if(countera == 0){
			Level_2_Read_Miss +=1;
			for(i=0; i<Level_2_Assoc; i++){
				if(LEVEL2[Level_2_Index_Decimal][i].VALID == 0){
					standing = i;
					break;
				}
				if(LEVEL2[Level_2_Index_Decimal][i].VALID == 1){
					counterb +=1;
				}
			}
			if(counterb == Level_2_Assoc){
				standing = LEVEL2CachePolicy(LEVEL2);
			}
			if(LEVEL2[Level_2_Index_Decimal][standing].DIRTY ==1 && LEVEL2[Level_2_Index_Decimal][standing].VALID == 1){
				Level_2_Write_Back += 1;
			}
			LEVEL2[Level_2_Index_Decimal][standing].TAG = Level_2_Tag;
			LEVEL2[Level_2_Index_Decimal][standing].POLICY_COUNTER = Level_2_Policy_counter;
			LEVEL2[Level_2_Index_Decimal][standing].DIRTY = 0;
			LEVEL2[Level_2_Index_Decimal][standing].VALID = 1;
		}
	}
}

int LEVEL2CachePolicy(CACHE LEVEL2[][32])
{
	int standing;
	if(strcasecmp(Level_2_Policy,"0") == 0 || strcasecmp(Level_2_Policy,"2") == 0 || strcasecmp(Level_2_Policy,"3") == 0)
		standing = LRUPolicy2(LEVEL2);
	else if (strcasecmp(Level_2_Policy,"1") == 0)
		standing = LRUPolicy2(LEVEL2);
	
	return standing;
}

int LRUPolicy2(CACHE LEVEL2[][32])
{
	int i, LRU2 = 0, WAY_LRU2 = 0;
	LRU2 = LEVEL2[Level_2_Index_Decimal][0].POLICY_COUNTER;
	for (i = 0; i < Level_2_Assoc; i++){
		if(LEVEL2[Level_2_Index_Decimal][i].POLICY_COUNTER < LRU2){
			LRU2 = LEVEL2[Level_2_Index_Decimal][i].POLICY_COUNTER;
			WAY_LRU2 = i;
		}
	}
	return WAY_LRU2;
}

void LEVEL2WriteCache(CACHE LEVEL1[][32], CACHE LEVEL2[][32], int standing)
{
	unsigned long long Decode_add = 0, tagz = 0, Level_2_Tag = 0;
	unsigned long long Level_2_Index_Decimal = 0;

	tagz = LEVEL1[Level_1_Index_Decimal][standing].TAG;
	Decode_add = ( (( (tagz << Level_1_Index_Width) | (Level_1_Index_Decimal)) << (Level_1_Offset_Width)) | (Level_1_Offset_Decimal) );
	Level_2_Tag = Decode_add >> (Level_2_Index_Width + Level_2_Offset_Width);
	Level_2_Index_Decimal = ( Decode_add << Level_2_Tag_Width) >> (Level_2_Tag_Width + Level_2_Offset_Width);

	int i, countera = 0, counterb = 0;
	int standing2 = 0;
	Level_2_Write += 1;
	Level_2_Policy_counter += 1;

	if(strcasecmp(Level_2_Policy,"0") == 0 || strcasecmp(Level_2_Policy,"2") == 0 || strcasecmp(Level_2_Policy,"3") == 0){
		for(i=0; i<Level_2_Assoc; i++){
			if(LEVEL2[Level_2_Index_Decimal][i].VALID == 1 && (Level_2_Tag == LEVEL2[Level_2_Index_Decimal][i].TAG)){
				Level_2_Write_Hit += 1;
				countera = 1;
				LEVEL2[Level_2_Index_Decimal][i].POLICY_COUNTER = Level_2_Policy_counter;
				LEVEL2[Level_2_Index_Decimal][i].DIRTY = 1;
				break;
			}
		}
		
	if(countera == 0){
		Level_2_Write_Miss +=1;
		for(i=0; i<Level_2_Assoc; i++){
			if(LEVEL2[Level_2_Index_Decimal][i].VALID == 0){
				standing2 = i;
				break;
			}
			else
				counterb += 1;
		}
		if(counterb == Level_2_Assoc){
			standing2 = LEVEL2WriteCachePolicy(LEVEL2, Level_2_Index_Decimal);
		}
		if(LEVEL2[Level_2_Index_Decimal][standing2].DIRTY ==1 && LEVEL2[Level_2_Index_Decimal][standing].VALID == 1){
			Level_2_Write_Back += 1;
		}
		LEVEL2[Level_2_Index_Decimal][standing2].TAG = Level_2_Tag;
		LEVEL2[Level_2_Index_Decimal][standing2].POLICY_COUNTER = Level_2_Policy_counter;
		LEVEL2[Level_2_Index_Decimal][standing2].DIRTY = 0;
		LEVEL2[Level_2_Index_Decimal][standing2].VALID = 1;
	}
	}
	else if(strcasecmp(Level_2_Policy,"1") == 0){
			for(i=0; i<Level_2_Assoc; i++){
			if(LEVEL2[Level_2_Index_Decimal][i].VALID == 1 && LEVEL2[Level_2_Index_Decimal][i].TAG == Level_2_Tag){
				Level_2_Write_Hit += 1;
				countera = 1;
				LEVEL2[Level_2_Index_Decimal][i].DIRTY = 1;
				break;
			}
		}
		if(countera == 0){
			Level_2_Write_Miss +=1;
			for(i=0; i<Level_2_Assoc; i++){
				if(LEVEL2[Level_2_Index_Decimal][i].VALID == 0){
					standing2 = i;
					break;
				}
				if(LEVEL2[Level_2_Index_Decimal][i].VALID == 1){
					counterb +=1;
				}
			}
			if(counterb == Level_2_Assoc){
				standing2 = LEVEL2WriteCachePolicy(LEVEL2, Level_2_Index_Decimal);
			}
			if(LEVEL2[Level_2_Index_Decimal][standing2].DIRTY ==1 && LEVEL2[Level_2_Index_Decimal][standing].VALID == 1){
				Level_2_Write_Back += 1;
			}
			LEVEL2[Level_2_Index_Decimal][standing2].TAG = Level_2_Tag;
			LEVEL2[Level_2_Index_Decimal][standing2].POLICY_COUNTER = Level_2_Policy_counter;
			LEVEL2[Level_2_Index_Decimal][standing2].DIRTY = 0;
			LEVEL2[Level_2_Index_Decimal][standing2].VALID = 1;
		}
	}
}

int LEVEL2WriteLRU(CACHE LEVEL2[][32], int Level_2_Index_Decimal)
{
	int i, LRU2 = 0, WAY_LRU2 = 0;
	LRU2 = LEVEL2[Level_2_Index_Decimal][0].POLICY_COUNTER;
	for (i = 0; i < Level_2_Assoc; i++){
		if(LEVEL2[Level_2_Index_Decimal][i].POLICY_COUNTER < LRU2){
			LRU2 = LEVEL2[Level_2_Index_Decimal][i].POLICY_COUNTER;
			WAY_LRU2 = i;
		}
	}
	return WAY_LRU2;
}

int LEVEL2WriteCachePolicy(CACHE LEVEL2[][32], int Level_2_Index_Decimal)
{
	int standing = 0;
	if(strcasecmp(Level_2_Policy,"0") == 0|| strcasecmp(Level_2_Policy,"2") == 0 || strcasecmp(Level_2_Policy,"3") == 0 )
		standing = LEVEL2WriteLRU(LEVEL2, Level_2_Index_Decimal);
	else if(strcasecmp(Level_2_Policy, "1") == 0)
		standing = LEVEL2WriteLRU(LEVEL2, Level_2_Index_Decimal);
	return standing;
}