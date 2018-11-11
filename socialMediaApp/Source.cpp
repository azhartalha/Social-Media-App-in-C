#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include "memoryManager.h"
#include <windows.h>
#define UserDataStartPoint 0
#define CommentsDataStartPoint 1048324
#define PostsDataStartPoint 2096792

void init()
{
	int start = 4;
	FILE * fp = fopen("DB.txt", "rb+");
	fwrite(&start, sizeof(int), 1, fp);
	
	fseek(fp, CommentsDataStartPoint, SEEK_SET);
	start = CommentsDataStartPoint + 4;
	fwrite(&start, sizeof(int), 1, fp);
	
	fseek(fp, PostsDataStartPoint, SEEK_SET);
	start = PostsDataStartPoint + 4;
	fwrite(&start, sizeof(int), 1, fp);
}

void createEmpty100mbFile()
{
	FILE * fp = fopen("DB.txt", "wb");
	int * buff = (int *)azMalloc(8192);
	for (int i = 0; i < 2048; i++)
	{
		buff[i] = 0;
	}
	for (int i = 0; i < 12800; i++)
		fwrite(buff, 8192, 1, fp);
	azFree(buff);
	fclose(fp);
	init();
}

void stringCopy(char * des, char *src)
{
	int i;
	for (i = 0; src[i] != '\0'; i++)
	{
		des[i] = src[i];
	}
	des[i] = '\0';
	return;
}

int compareStrings(char *str1, char *str2)
{
	int i;
	for (i = 0; str1[i] != '\0' && str2[i] != '\0'; i++)
	{
		if (str1[i] != str2[i])
			return 0;
	}
	if (str1[i] == str2[i])
		return 1;
	return 0;
}

void print(char *str)
{
	for (int i = 0; str[i] != '\0'; i++)
		printf("%c", str[i]);
}

int stringLength(char  *str)
{
	int len;
	for (len = 0; str[len] != '\0'; len++);
	return len;
}
/*-------------------------------------------------------------------------------------*/
//User Module
typedef struct userStruct
{
	int flag = 0; //Deleted Flag
	//Twitter standards
	char userName[32];	
	char password[32];
	char email[32];
	char description[128];
};

int searchUserName(char * userName)
{
	FILE * fr = fopen("DB.txt", "rb");
	int end;
	fread(&end, sizeof(int), 1, fr);
	userStruct * user = (userStruct *)azMalloc(sizeof(user));
	int curr = 4;
	while (curr < end)
	{
		fread(user, sizeof(userStruct), 1, fr);
		if (user->flag == 0 && compareStrings(userName, user->userName))
		{
			azFree(user);
			fclose(fr);
			return curr;
		}
		curr += sizeof(userStruct);
	}
	azFree(user);
	fclose(fr);
	return -1;
}
 

void createUser()
{
	userStruct user;
	while (1)
	{
		printf("Enter a new user name: ");
		scanf("%s", user.userName);
		if (searchUserName(user.userName) != -1)
		{
			int op;
			printf("\nA user Already exists with the name\nDo you want to try again or no (1/0)?:");
			scanf("%d", &op);
			if (op != 1)
				return;
		}
		else
			break;
	}
	printf("\nEnter the password: ");
	scanf("%s", user.password);
	printf("\nEnter user email: ");
	scanf("%s", user.email);
	printf("\nDo you want to add any descreption to your profile (1/0)?");
	int op;
	scanf("%d", &op);
	user.description[0] = '\0';
	if (op == 1)
	{
		int c;
		while ((c = getchar()) != EOF && c != '\n');
		gets_s(user.description);	
	}
	FILE * fp = fopen("DB.txt", "rb+");
	int end;
	fread(&end, sizeof(int), 1, fp);
	fseek(fp, end, SEEK_SET);
	fwrite(&user, sizeof(userStruct), 1, fp);
	end += sizeof(userStruct);
	fseek(fp, UserDataStartPoint, SEEK_SET);
	fwrite(&end, sizeof(int), 1, fp);
	fclose(fp);
	printf("\nAccount created\n");
}

int loggin(char * username)
{
	int curr = searchUserName(username);
	if (curr == -1)
	{
		printf("\nUsername not found\n");
		return 0;
	}
	userStruct user;
	FILE * fr = fopen("DB.txt", "rb");
	fseek(fr, curr, SEEK_SET);
	fread(&user, sizeof(userStruct), 1, fr);
	char password[32];
	printf("Enter the password: ");
	scanf("%s", password);
	if (compareStrings(password, user.password))
		return 1;
	else
	{
		printf("\nWrong password, please try again\n");
		return 0;
	}
}

void viewUser(char * userName)
{
	int curr = searchUserName(userName);
	if (curr == -1)
	{
		printf("\nNo user user found with the name");
		return;
	}
	userStruct user;
	FILE * fp = fopen("DB.txt", "rb");
	fseek(fp, curr, SEEK_SET);
	fread(&user, sizeof(userStruct), 1, fp);
	printf("\nusername:%s   email:%s   desc:%s", user.userName, user.email, user.description);
	fclose(fp);
	return;
}

void updateUser(char * username)
{
	userStruct user;
	printf("\nEnter your username: ");
	compareStrings(user.userName, username);
	scanf("%s", user.userName);
	int curr = searchUserName(user.userName);
	if (curr == -1)
	{
		printf("User name not found");
		return;
	}
	printf("\nEnter the new password: ");
	scanf("%s", user.password);
	printf("\nEnter the new email: ");
	scanf("%s", user.email);
	printf("\nEnter the new description: ");
	int c;
	while ((c = getchar()) != EOF && c != '\n');
	gets_s(user.description);
	FILE * fp = fopen("DB.txt", "rb+");
	fseek(fp, curr, SEEK_SET);
	fwrite(&user, sizeof(userStruct), 1, fp);
	fclose(fp);
}

void deleteUser(char * userName)
{
	int curr = searchUserName(userName);
	if (curr == -1)
	{
		printf("\nUser name not found\n");
		return;
	}
	FILE * fp = fopen("DB.txt", "rb+");
	fseek(fp, curr, SEEK_SET);
	userStruct user;
	fread(&user, sizeof(user), 1, fp);
	fseek(fp, curr, SEEK_SET);
	user.flag = 1;
	fwrite(&user, sizeof(user), 1, fp);
	fclose(fp);
	return;
}
/*---------------------------------------------------------------------------------------*/
// Blob (or) posts module

//Before storing any file into our fs blob. A meta data is maintained which will help to track 
//a post and at the same time optimizes operations on posts.
typedef struct postStruct
{
	int postID;
	int size;
	int deleted = 0;
	int like = 0;
	char username[32];
	char postName[64];
};

int seachByPostId(int postId)
{
	int curr = PostsDataStartPoint + 4;
	postStruct post;
	FILE * fp = fopen("DB.txt", "rb");
	fseek(fp, PostsDataStartPoint, SEEK_SET);
	int end;
	fread(&end, sizeof(int), 1, fp);
	while (curr < end)
	{
		fread(&post, sizeof(postStruct), 1, fp);
		if (post.deleted == 0 && post.postID == postId)
		{
			fclose(fp);
			return curr;
		}
		fseek(fp, post.size, SEEK_CUR);
		curr += sizeof(postStruct) + post.size;
	}
	fclose(fp);
	return -1;
}

void downloadFile(int postId)
{
	int curr = seachByPostId(postId);
	if (curr == -1)
	{
		printf("\nInvalid post ID, view posts before coming here to know the post ID. also you can comment and like posts there\n");
		return;
	}
	postStruct post;
	FILE * fr = fopen("DB.txt", "rb");
	fseek(fr, curr, SEEK_SET);
	fread(&post, sizeof(postStruct), 1, fr);
	int fileSize = post.size;
	printf("\nDo you want to store in newlocation or with postName?(1/0)");
	char path[64];
	int op;
	scanf("%d", &op);
	if (op)
	{
		printf("\nEnter the path with file extension: ");
		scanf("%s", path);
	}
	else
	{
		stringCopy(path, post.postName);
	}
	FILE * fw = fopen(path, "wb");
	void *buff = azMalloc(100);
	for (int i = 0; i < fileSize / 100; i++)
	{
		fread(buff, 100, 1, fr);
		fwrite(buff, 100, 1, fw);
	}
	fread(buff, fileSize % 100, 1, fr);
	fwrite(buff, fileSize % 100, 1, fw);
	azFree(buff);
	fclose(fr);
	fclose(fw);
}

void showPostsByUser(char * userName)
{
	printf("\n");
	int curr = PostsDataStartPoint + 4;
	postStruct post;
	FILE * fp = fopen("DB.txt", "rb");
	fseek(fp, PostsDataStartPoint, SEEK_SET);
	int end;
	fread(&end, sizeof(int), 1, fp);
	while (curr < end)
	{
		fread(&post, sizeof(postStruct), 1, fp);
		if (post.deleted == 0 && compareStrings(post.username, userName))
			printf("%d %s %s %d\n", post.postID, post.postName, post.username, post.like);

		fseek(fp, post.size, SEEK_CUR);
		curr += sizeof(postStruct) + post.size;
	}
	fclose(fp);
	return;
}

void showPosts()
{
	printf("\n");
	printf("post ID       post Name           user who posted         number of likes\n");
	int curr = PostsDataStartPoint + 4;
	postStruct post;
	FILE * fp = fopen("DB.txt", "rb");
	fseek(fp, PostsDataStartPoint, SEEK_SET);
	int end;
	fread(&end, sizeof(int), 1, fp);
	while (curr < end)
	{
		fread(&post, sizeof(postStruct), 1, fp);
		if (post.deleted == 0)
			printf("%d %s %s %d\n", post.postID, post.postName, post.username, post.like);

		fseek(fp, post.size, SEEK_CUR);
		curr += sizeof(postStruct) + post.size;
	}
	fclose(fp);
	return;
}

void likeApost(int postId)
{
	int curr = seachByPostId(postId);
	if (curr == -1)
	{
		printf("\nNo such post found\n");
		return;
	}
	postStruct post;
	FILE * fp = fopen("DB.txt", "rb+");
	fseek(fp, curr, SEEK_SET);
	fread(&post, sizeof(postStruct), 1, fp);
	post.like++;
	fseek(fp, curr, SEEK_SET);
	fwrite(&post, sizeof(postStruct), 1, fp);
	fclose(fp);
}

void deletePost(int postId)
{
	int curr = seachByPostId(postId);
	if (curr == -1)
	{
		printf("\nNo such post found\n");
		return;
	}
	postStruct post;
	FILE * fp = fopen("DB.txt", "rb+");
	fseek(fp, curr, SEEK_SET);
	fread(&post, sizeof(postStruct), 1, fp);
	post.deleted = 1;
	fseek(fp, curr, SEEK_SET);
	fwrite(&post, sizeof(postStruct), 1, fp);
	fclose(fp);
}

void uploadFile(char * username)
{
	postStruct post, prev;
	printf("\nEnter the file path: ");
	scanf("%s", post.postName);
	FILE * fr = fopen(post.postName, "rb");
	if (fr == NULL)
	{
		printf("\nFile not found\n");
		return;
	}
	fseek(fr, 0, SEEK_END);
	int fileSize = ftell(fr);
	fseek(fr, 0, SEEK_SET);
	FILE * fw = fopen("DB.txt", "rb+");
	fseek(fw, PostsDataStartPoint, SEEK_SET);
	int end;
	fread(&end, sizeof(int), 1, fw);

	if (end >= PostsDataStartPoint + sizeof(postStruct))
	{
		fseek(fw, end - sizeof(postStruct), SEEK_SET);
		fread(&prev, sizeof(postStruct), 1, fw);
		post.postID = prev.postID + 1;
	}
	else
		post.postID = 1;
	fseek(fw, end, SEEK_SET);
	post.like = 0;
	post.size = fileSize;
	stringCopy(post.username, username);
	fwrite(&post, sizeof(postStruct), 1, fw);
	void *buff = azMalloc(100);
	for (int i = 0; i < fileSize / 100; i++)
	{
		fread(buff, 100, 1, fr);
		fwrite(buff, 100, 1, fw);
	}
	fread(buff, fileSize % 100, 1, fr);
	fwrite(buff, fileSize % 100, 1, fw);
	azFree(buff);
	end += post.size + sizeof(postStruct);
	fseek(fw, PostsDataStartPoint, SEEK_SET);
	fwrite(&end, sizeof(int), 1, fw);
	fclose(fr);
	fclose(fw);
}
/*---------------------------------------------------------------------------------------*/
//Comments Modeule
typedef struct commentStruct
{
	int commentId;
	int postId;
	int deleted;
	char username[32];
	char data[100];
};

void addComments(int postId, char * username)
{	
	commentStruct comment, prev;
	if (seachByPostId(postId) == -1)
	{
		printf("\nInvalid postID");
		return;
	}
	FILE * fw = fopen("DB.txt", "rb+");
	fseek(fw, CommentsDataStartPoint, SEEK_SET);
	int end;
	fread(&end, sizeof(int), 1, fw);

	if (end >= CommentsDataStartPoint + sizeof(commentStruct))
	{
		fseek(fw, end - sizeof(commentStruct), SEEK_SET);
		fread(&prev, sizeof(commentStruct), 1, fw);
		comment.commentId = prev.commentId + 1;
	}
	else
		comment.commentId = 1;
	comment.postId = postId;
	stringCopy(comment.username, username);
	comment.deleted = 0;
	printf("Enter the comment: ");
	int c;
	while ((c = getchar()) != EOF && c != '\n');
	gets_s(comment.data);
	fseek(fw, end, SEEK_SET);
	fwrite(&comment, sizeof(commentStruct), 1, fw);
	end += sizeof(commentStruct);
	fseek(fw, CommentsDataStartPoint, SEEK_SET);
	fwrite(&end, sizeof(int), 1, fw);
	fclose(fw);
}

void showCommentsByPostId(int postId)
{
	int curr = CommentsDataStartPoint + 4;
	commentStruct comment;
	FILE * fp = fopen("DB.txt", "rb");
	fseek(fp, CommentsDataStartPoint, SEEK_SET);
	int end;
	fread(&end, sizeof(int), 1, fp);
	while (curr < end)
	{
		fread(&comment, sizeof(commentStruct), 1, fp);
		if (comment.deleted == 0 && comment.postId == postId)
		{
			printf("\nUser name: %s, commented %s on the post %d\n", comment.username, comment.data, comment.postId);
		}
		curr += sizeof(commentStruct);
	}
	fclose(fp);
	return;
}

int searchCommentByCommentID(int commentId)
{
	int curr = CommentsDataStartPoint + 4;
	commentStruct comment;
	FILE * fp = fopen("DB.txt", "rb");
	fseek(fp, CommentsDataStartPoint, SEEK_SET);
	int end;
	fread(&end, sizeof(int), 1, fp);
	while (curr < end)
	{
		fread(&comment, sizeof(commentStruct), 1, fp);
		if (comment.deleted == 0 && comment.commentId == commentId)
		{
			fclose(fp);
			return curr;
		}
		curr += sizeof(commentStruct);
	}
	fclose(fp);
	return -1;
}

void deleteComment(int commentId)
{
	int curr = searchCommentByCommentID(commentId);
	if (curr == -1)
	{
		printf("\nNo such post found\n");
		return;
	}
	commentStruct comment;
	FILE * fp = fopen("DB.txt", "rb+");
	fseek(fp, curr, SEEK_SET);
	fread(&comment, sizeof(commentStruct), 1, fp);
	comment.deleted = 1;
	fseek(fp, curr, SEEK_SET);
	fwrite(&comment, sizeof(commentStruct), 1, fp);
	fclose(fp);
}

void updateComment(int commentId)
{
	int curr = searchCommentByCommentID(commentId);
	if (curr == -1)
	{
		printf("\nNo such post found\n");
		return;
	}
	commentStruct comment;
	FILE * fp = fopen("DB.txt", "rb+");
	fseek(fp, curr, SEEK_SET);
	fread(&comment, sizeof(commentStruct), 1, fp);
	printf("\nEnter the comment to be updated: ");
	int c;
	while ((c = getchar()) != EOF && c != '\n');
	gets_s(comment.data);
	fseek(fp, curr, SEEK_SET);
	fwrite(&comment, sizeof(commentStruct), 1, fp);
	fclose(fp);
}
/*-----------------------------------------------------------------------------------*/
//UI 

void commentsUI(char * username)
{
	int op;
	while (1)
	{
		printf("\n1. add comment\n2. viewComments of a post\n3. update comment\n4. deletecomment\n5.back");
		scanf("%d", &op);
		if(op == 1)
		{
			int postID;
			printf("Enter the post Id\n");
			scanf("%d", &postID);
			addComments(postID, username);
		}
		else if (op == 2)
		{
			int postID;
			printf("Enter the post Id\n");
			scanf("%d", &postID);
			showCommentsByPostId(postID);
		}
		else if (op == 3)
		{
			int commentId;
			printf("\nEnter the comment ID, comment ids are incremental automatically\n");
			scanf("%d", &commentId);
			updateComment(commentId);
		}
		else if (op == 4)
		{
			int commentId;
			printf("\nEnter the comment ID, comment ids are incremental automatically\n");
			scanf("%d", &commentId);
			deleteComment(commentId);
		}
		else
		{
			break;
		}
	}
}

void commentingAndLiking(char * username)
{
	int op;
	while (1)
	{
		printf("\n\n1.comment on a post\n2.Like a post\n3.Back\n");
		scanf("%d", &op);
		if (op == 1)
		{
			commentsUI(username);
		}
		else if (op == 2)
		{
			printf("Enter the post Id of the post to like:");
			int postId;
			scanf("%d", &postId);
			likeApost(postId);
		}
		else
		{
			break;
		}
	}
}
void session(char * username)
{
	int op;
	while (1)
	{
		printf("\nLogged In\n");
		printf("\Note:post ids and comment Ids are given in incremental order with file persistance\n");
		printf("\n1.update profile\n2.Upload File\n3.Download File\n4.View post\n5.View user\n6.delete post\n7.delete profile\n8.exit\n");
		scanf("%d", &op);
		if (op == 1)
		{
			updateUser(username);
			printf("\nPrifile updated\n");
		}
		else if (op == 2)
		{
			uploadFile(username);
		}
		else if (op == 3)
		{
			printf("\nEnter the post id to download\n");
			int postid;
			scanf("%d", &postid);
			downloadFile(postid);
		}
		else if (op == 4)
		{
			showPosts();
			commentingAndLiking(username);
		}
		else if (op == 5)
		{
			printf("Enter the user name: ");
			char un[32];
			scanf("%s", un);
			viewUser(un);
			printf("\n He posted:\n");
			showPostsByUser(un);
		}
		else if (op == 6)
		{
			printf("\nEnter the post ID");
			int postId;
			scanf("%d", &postId);
			deletePost(postId);
		}
		else if (op == 7)
		{
			deleteUser(username);
			break;
		}
		else
		{
			break;
		}
	}
}

void UI()
{
	int op;
	while (1)
	{
		printf("\n1.Create account\n2.Loggin\n3.exit\n\n");
		scanf("%d", &op);
		if (op == 1)
		{
			createUser();
		}
		else if (op == 2)
		{
			char userName[32];
			printf("Enter username: ");
			scanf("%s", userName);
			if (loggin(userName))
				session(userName);
		}
		else
		{
			break;
		}
	}
	return;
}
/*-----------------------------------------------------------------------------------*/
int main()
{
	initMemory(); //Please check the memeoryManager.h to knwo what this fucntion does;
	FILE * fp = fopen("DB.txt", "rb");
	if (fp == NULL)
		createEmpty100mbFile();
	UI();
	getchar();
}