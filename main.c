/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/06 18:32:56 by mjacq             #+#    #+#             */
/*   Updated: 2020/07/14 18:53:16 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

// Libasm prototypes{{{

// MANDATORY

size_t	ft_strlen(const char *str);
char	*ft_strcpy(char *dest, const char *src);
int		ft_strcmp(const char *s1, const char *s2);
ssize_t	ft_write(int fd, const void *buf, size_t count);
ssize_t	ft_read(int fd, void *buf, size_t count);
char	*ft_strdup(const char *s);

// BONUSES

int	ft_atoi_base(char *str, char *base);

typedef struct		s_list
{
	void			*data;
	struct s_list	*next;
}					t_list;

void	ft_list_push_front(t_list **begin_list, void *data);
int		ft_list_size(t_list *lst);
void	ft_list_sort(t_list **begin_list, int (*cmp)());
void	ft_list_remove_if(t_list **begin_list, void *data_ref, int (*cmp)());

/*}}}*/
// Test general functions{{{
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define GREY "\e[38;2;128;128;128m"
#define BLUE "\e[34m"
#define BOLD "\e[1m"
#define RST "\e[0m"

void	ft_putstr(const char *s)
{
	while (*s)
		write(1, s++, 1);
}

void	test_title(const char *title)
{
	ft_putstr(BLUE"\n/*\n** Test function : ");
	ft_putstr(title);
	ft_putstr(BLUE"\n*/\e[0m\n\n");
}

void	test_description(const char *description)
{
	ft_putstr("\e[33m");
	ft_putstr(description);
	ft_putstr(RST "\n");
}

void	newline(void)
{
	ft_putstr("\n");
}

#define TEST(CODE_BEFORE,X)	\
	do { CODE_BEFORE; \
		if (X) { ft_putstr("\e[32m✓\e[0m"); } \
		else { ft_putstr("\e[31m✗\e[0m"); } \
		ft_putstr("\t Test : " GREY #CODE_BEFORE RST " " #X "\n"); \
	} while (0)

#define TEST_PRINT_AND_DO(X)	do { ft_putstr(GREY #X "\n"RST); X } while (0)

/* #define INTRO(TEXT, CODE) do { ft_putstr("\n" GREY TEXT); CODE ft_putstr(#CODE "\n" RST); } while (0) */

void	ft_comment(const char *s)
{
	ft_putstr(GREY);
	ft_putstr(s);
	ft_putstr(RST);
}

#define OPEN_TWO_FILES \
	int	fd1; \
	int	fd2; \
	char	s1[10]; \
	char	s2[10]; \
	fd1 = open("tests/text1.txt", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); \
	fd2 = open("tests/text2.txt", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);

#define OFFSETS_TO_ORIGIN lseek(fd1, 0, SEEK_SET); lseek(fd2, 0, SEEK_SET);

#define CLOSE_FILES close(fd1); close(fd2);

void	test_exemple(void)
{
	test_title("Exemple de test vrai / faux");
	TEST(int a = 1; int b = 1;, a == b);
	TEST(int a = 1; int b = 0;, a == b);
}

/*}}}*/
// Tests{{{

// strlen{{{
void	strlen_test(void)
{
	test_title("strlen");
	TEST(const char *s = "Hello";, ft_strlen(s) == strlen(s));
	TEST(const char *s = " Hello World ";, ft_strlen(s) == strlen(s));
	TEST(const char *s = "H";, ft_strlen(s) == strlen(s));
	TEST(const char *s = "";, ft_strlen(s) == strlen(s));
}
/*}}}*/

// write{{{
#define WRITE_TEST(FD, STR, LEN) \
	ft_putstr(GREY "Write comparison : \""BOLD STR RST GREY"\" " \
		"(fd="#FD", len="BOLD #LEN RST GREY")\n"); \
	TEST_PRINT_AND_DO( \
	const char *str = STR; \
	ft_write(FD, str, LEN); \
	newline(); \
	write(FD, str, LEN); \
	newline(); \
	)

void	write_test(void)
{
	test_title("write");

	// Print on stdin
	WRITE_TEST(1, "hello", strlen("hello"));
	WRITE_TEST(1, "hello", 1);
	WRITE_TEST(1, "hello", 0);
	WRITE_TEST(1, "hello", 6);
	WRITE_TEST(1, "🍉🍌🍍", strlen("🍉🍌🍍"));
	WRITE_TEST(1, "🤩😲🙊🙈🤢🤮", strlen("🤩😲🙊🙈🤢🤮") - 1);

	// Print elsewhere
	ft_comment("\nTest write on file descriptors other than stdin.\n");
	ft_comment("Test valid files. errno set before at 42 should not change.\n");
	TEST(
			OPEN_TWO_FILES;
			int	n = 6;
			const char *s = "world";
			errno = 42; // change errno first to see if it changes (should not be forced to turn into 0)
			int	ret1 = ft_write(fd1, s, n);
			int	err1 = errno;
			int	ret2 = write(fd2, s, n);
			int	err2 = errno;
			OFFSETS_TO_ORIGIN;
			read(fd1, s1, n);
			read(fd2, s2, n);
			CLOSE_FILES;
			printf("s1 = %s, s2 = %s\n", s1, s2);
			printf("err1 = %d, err2 = %d\n", err1, err2);
			,
			(!memcmp(s1, s2, n) && ret1 == ret2 && err1 == err2)
		);

	// Test returns
	ft_comment("\nWith fd = -1 : test return and errno.\n");
	TEST(
			int	fd = -1;
			int	n = 6;
			errno = 0;
			int	ret1 = ft_write(fd, "world", n);
			int	err1 = errno;
			int	ret2 = write(fd, "world", n);
			int	err2 = errno;
			printf("ret1 = %d, ret2 = %d\n", ret1, ret2);
			printf("err1 = %d, err2 = %d\n", err1, err2);
			,
			(ret1 == ret2 && err1 == err2)
		);
}
/*}}}*/

// read{{{
void	read_test(void)
{
	test_title("read");

	TEST(
			OPEN_TWO_FILES;
			int	n = 6;
			const char *s = "world";
			write(fd1, s, n);
			write(fd2, s, n);
			OFFSETS_TO_ORIGIN;
			errno = 42;
			int	ret1 = ft_read(fd1, s1, n);
			int	err1 = errno;
			int	ret2 = read(fd2, s2, n);
			int	err2 = errno;
			CLOSE_FILES;
			printf("s1 = %s, s2 = %s\n", s1, s2);
			,
			(!memcmp(s1, s2, n) && ret1 == ret2 && err1 == err2)
		);

	ft_comment("\nWith fd = -1 : test return and errno.\n");
	TEST(
			int	fd = -1;
			int	n = 6;
			char	s[10];
			errno = 0;
			int	ret1 = ft_read(fd, s, n);
			int	err1 = errno;
			int	ret2 = read(fd, s, n);
			int	err2 = errno;
			printf("ret1 = %d, ret2 = %d\n", ret1, ret2);
			printf("err1 = %d, err2 = %d\n", err1, err2);
			,
			(ret1 == ret2 && err1 == err2)
		);
}
/*}}}*/

// strcpy{{{

void	test_strcpy_item(const char *s)
{
	TEST( \
			char d1[20];
			char d2[20];
			char *ret1;
			char *ret2;
			ret1 = ft_strcpy(d1, s);
			ret2 = strcpy(d2, s);
			printf("d1 = \"%s\", d2 = \"%s\"\n", d1, d2);
			,
			!strcmp(d1, d2) && ret1 == d1 && ret2 == d2 && ret1 != ret2);
}

void	strcpy_test(void)
{
	test_title("strcpy");
	test_strcpy_item("Hello");
	test_strcpy_item("W");
	test_strcpy_item("");
	test_strcpy_item("🛸👽🛸");
}

/*}}}*/

// strcmp{{{

void	strcmp_test(void)
{
	test_title("strcmp");
	TEST(char *s1 = "Hello"; char *s2 = "Hello"; printf("ft_strcmp:%d, strcmp:%d\n", ft_strcmp(s1, s2), strcmp(s1, s2));, ft_strcmp(s1, s2) == strcmp(s1, s2));
	TEST(char *s1 = "Hllo"; char *s2 = "Hello"; printf("ft_strcmp:%d, strcmp:%d\n", ft_strcmp(s1, s2), strcmp(s1, s2));, ft_strcmp(s1, s2) == strcmp(s1, s2));
	TEST(char *s1 = "Hello"; char *s2 = "hello"; printf("ft_strcmp:%d, strcmp:%d\n", ft_strcmp(s1, s2), strcmp(s1, s2));, ft_strcmp(s1, s2) == strcmp(s1, s2));
	TEST(char *s1 = ""; char *s2 = "hello"; printf("ft_strcmp:%d, strcmp:%d\n", ft_strcmp(s1, s2), strcmp(s1, s2));, ft_strcmp(s1, s2) == strcmp(s1, s2));
	TEST(char *s1 = "Hello"; char *s2 = ""; printf("ft_strcmp:%d, strcmp:%d\n", ft_strcmp(s1, s2), strcmp(s1, s2));, ft_strcmp(s1, s2) == strcmp(s1, s2));
	TEST(char *s1 = ""; char *s2 = ""; printf("ft_strcmp:%d, strcmp:%d\n", ft_strcmp(s1, s2), strcmp(s1, s2));, ft_strcmp(s1, s2) == strcmp(s1, s2));
	/* TEST(char *s1 = 0; char *s2 = 0; printf("ft_strcmp:%d, strcmp:%d\n", ft_strcmp(s1, s2), strcmp(s1, s2));, ft_strcmp(s1, s2) == strcmp(s1, s2)); // must segfault (strcmp is not protected) */
}

/*}}}*/

// strdup{{{

void	strdup_test_item(const char *s)
{
	TEST(printf("s=\"%s\"\n", s); char *s1 = ft_strdup(s); char *s2 = strdup(s);, !strcmp(s1, s2) && (s1 != s2 || (!s1 && !s2)) && (s1 != s || (!s1 && !s)));
}

void	strdup_test(void)
{
	test_title("strdup");

	strdup_test_item("Hello");
	strdup_test_item("Hello world");
	strdup_test_item("");
	//TODO : REMOVE FLAG -Werror
	/* strdup_test_item(0); */
	TEST(char *s = ft_strdup("Hello"); free(s);, "No error when free");
}
/*}}}*/

/*}}}*/
// Bonus{{{
#ifdef BONUS
// Bonus descriptions :{{{

// atoi_base{{{

/* • Écrire une fonction qui renvoie un nombre. Ce nombre est connu sous la forme d’une */
/* chaîne de caractères. */
/* • La chaîne de caractères exprime le nombre dans une base particulière passée en */
/* second paramètre. */
/* • La fonction doit gérer les nombres négatifs. */
/* • La fonction doit les signes comme man atoi. */
/* • Si un paramètre contient une erreur la fonction renvoie 0. Une erreur peut être : */
/* ◦ str est une chaîne vide ; */
/* ◦ la base est vide ou est de taille 1 ; */
/* ◦ str contient des caractères qui ne sont pas dans la base et ne sont pas + ou - ; */
/* ◦ la base contient deux fois le même caractère ; */
/* ◦ la base contient les caractères + ou - ; */
/* ◦ etc. */
/* • Elle devra être prototypée de la façon suivante : */
/*   int ft_atoi_base(char *str, char *base); */
/*}}}*/

// ft_list_push_front{{{

/* Écrire la fonction ft_list_push_front qui ajoute au début de la liste un nouvel */
/* élément de type t_list. */
/* • Elle devra assigner data au paramètre fourni. */
/* • Elle mettra à jour, si nécéssaire, le pointeur sur le début de liste. */
/* • Elle devra être prototypée de la façon suivante : */
/* void ft_list_push_front(t_list **begin_list, void *data); */
/*}}}*/

// ft_list_size{{{

/* Écrire la fonction ft_list_size qui renvoie le nombre d’éléments dans la liste. */
/* • Elle devra être prototypée de la façon suivante : */
/* int ft_list_size(t_list *begin_list); */

/*}}}*/

// ft_list_sort{{{

/* Écrire la fonction ft_list_sort qui trie par ordre croissant le contenu de la liste, */
/* en comparant deux maillons grâce à une fonction de comparaison de données des */
/* deux maillons. */
/* • Elle devra être prototypée de la façon suivante : */
/* void ft_list_sort(t_list **begin_list, int (*cmp)()); */

/*}}}*/

// ft_list_remove_if{{{

/* • Écrire la fonction ft_list_remove_if qui efface de la liste tous les élements dont */
/* la donnée est "égale" à la donnée de référence. */
/* • Elle devra être prototypée de la façon suivante : */
/* void ft_list_remove_if(t_list **begin_list, void *data_ref, int (*cmp)()); */

/*}}}*/

 /*}}}*/

// Bonus c functions{{{
//

// ft_atoi_base{{{
static int	ft_atoi_invalid_base(const char *base)
{
	int	i;

	while (*base)
	{
		if (*base == '+' || *base == '-')
			return (1);
		i = 1;
		while (base[i])
		{
			if (*base == base[i++])
				return (1);
		}
		base++;
	}
	return (0);
}

int	ft_atoi_base_c(char *str, char *base)
{
	long	result;
	int		sign;
	int		base_len;
	int		i;

	if (!str || !base || !*str || (base_len = strlen(base)) < 2)
		return (0);
	if (ft_atoi_invalid_base(base))
		return (0);
	result = 0;
	sign = 1;
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else if (*str == '+')
		str++;
	while (*str)
	{
		i = 0;
		while (base[i] && *str != base[i])
			i++;
		if (!base[i])
			return (0);
		result *= base_len;
		result += i;
		if (sign * result > INT_MAX || sign * result < INT_MIN)
			return (0);
		str++;
	}
	return (sign * result);
}
/*}}}*/

t_list	*ft_create_elem(void *data)/*{{{*/
{
	t_list	*new;

	if ((new = (t_list *)malloc(sizeof(*new))) == NULL)
		return (NULL);
	new->data = data;
	new->next = NULL;
	return (new);
}/*}}}*/

void	ft_lst_push_front(t_list **begin_list, void *data)/*{{{*/
{
	t_list	*new;

	new = ft_create_elem(data);
	new->next = *begin_list;
	*begin_list = new;
}/*}}}*/

void	ft_lst_push_back(t_list **begin_list, void *data)/*{{{*/
{
	t_list	*tmp;

	if (!(*begin_list))
		*begin_list = ft_create_elem(data);
	else
	{
		tmp = *begin_list;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = ft_create_elem(data);
	}
}/*}}}*/

void	ft_lst_clear(t_list **begin_list)/*{{{*/
{
	t_list	*temp;

	while (*begin_list)
	{
		temp = (*begin_list)->next;
		free(*begin_list);
		*begin_list = temp;
	}
}/*}}}*/

int	ft_lst_size(t_list *lst)/*{{{*/
{
	int n;

	n = 0;
	while (lst)
	{
		lst = lst->next;
		n++;
	}
	return (n);
}/*}}}*/

void	ft_lst_sort(t_list **begin_list, int (*cmp)())/*{{{*/
{
	t_list	*i;
	t_list	*j;
	void	*temp_data;

	i = *begin_list;
	while (i)
	{
		j = i->next;
		while (j)
		{
			if (cmp(i->data, j->data) > 0)
			{
				temp_data = i->data;
				i->data = j->data;
				j->data = temp_data;
			}
			j = j->next;
		}
		i = i->next;
	}
}/*}}}*/

// lst_remove_if{{{

void	ft_lst_remove_next(t_list *begin_list)
{
	t_list	*temp;
	
	temp = begin_list->next;
	begin_list->next = begin_list->next->next;
	free(temp);
}

void	ft_lst_remove_first(t_list **begin_list)
{
	t_list	*temp;

	temp = *begin_list;
	*begin_list = (*begin_list)->next;
	free(temp);
}

void	ft_lst_remove_if(t_list **begin_list, void *data_ref, int (*cmp)())
{
	t_list *temp;

	while (*begin_list && !(cmp((*begin_list)->data, data_ref)))
		ft_lst_remove_first(begin_list);
	temp = *begin_list;
	while (*begin_list && temp->next)
	{
		if (!(cmp(temp->next->data, data_ref)))
			ft_lst_remove_next(temp);
		else
			temp = temp->next;
	}
}
/*}}}*/

/*}}}*/

// Test lst general functions{{{

t_list	*lst_create_sample(char	**stdlst)/*{{{*/
{
	t_list	*lst;

	lst = 0;
	if (!stdlst)
		return (0);
	while (*stdlst)
		ft_lst_push_back(&lst, *stdlst++);
	return (lst);
}/*}}}*/

void	lst_print(t_list *lst)/*{{{*/
{
	if (!lst)
	{
		ft_putstr("(null)");
		return ;
	}
	while (lst)
	{
		puts(lst->data);
		lst = lst->next;
	}
}
/*}}}*/

// lst_test{{{
/* void	lst_test(void) */
/* { */
/* 	t_list	*lst; */

/* 	lst = lst_create_sample((char *[]){"hello", "world", 0}); */
/* 	lst_print(lst); */
/* 	ft_lst_clear(&lst); */
/* } */
/*}}}*/

int		lst_eq(t_list *l1, t_list *l2)/*{{{*/
{
	if (!l1 && !l2)
	{
		puts("(RQ: Both lists are null.)");
		return (1);
	}
	else if (!l1 || !l2)
	{
		printf("One (only %s) of the lists is null.\n", (!l1 ? "list 1" : "list 2"));
		return (0);
	}
	else if (l1 == l2)
	{
		puts("Compared lists are the same pointer");
		return (0);
	}
	else if (ft_lst_size(l1) != ft_lst_size(l2))
	{
		printf("list 1 size : %d ; list 2 : %d\n", ft_lst_size(l1), ft_lst_size(l2));
		return (0);
	}
	int n;
	t_list	*l1_copy = l1;
	t_list	*l2_copy = l2;
	n = 0;
	while (l1)
	{
		n++;
		if (strcmp(l1->data, l2->data))
		{
			printf("Lists are different (at link no %d, l1 is %s and l2 is %s.\n", n, l1->data, l2->data);
			puts("List 1 is :");
			lst_print(l1_copy);
			puts("List 2 is :");
			lst_print(l2_copy);
			return (0);
		}
		l1 = l1->next;
		l2 = l2->next;
	}
	return (1);
}/*}}}*/

/*}}}*/

// list_size_test{{{

void	list_size_test(void)
{
	t_list	*lst;

	test_title("ft_list_size");
	test_description("\
Écrire la fonction ft_list_size qui renvoie le nombre d’éléments dans la liste. \n\
• Elle devra être prototypée de la façon suivante : \n\
\t\e[35mint ft_list_size(t_list *begin_list); \n\
");
	TEST(lst = 0;, ft_list_size(lst) == 0);
	TEST(lst = ft_create_elem(0);, ft_list_size(lst) == 1);
	TEST(lst = ft_create_elem("hello");, ft_list_size(lst) == 1);
	TEST(lst = lst_create_sample((char *[]){"world", 0});, ft_list_size(lst) == 1);
	TEST(lst = lst_create_sample((char *[]){"hello", "world", 0});, ft_list_size(lst) == 2);
	TEST(lst = lst_create_sample((char *[]){"hello", "world", 0});, ft_list_size(lst) == ft_lst_size(lst));
	TEST(lst = lst_create_sample((char *[]){"hello", "world", "1", "2", "3", 0});, ft_list_size(lst) == 5);
	TEST(lst = lst_create_sample((char *[]){0});, ft_list_size(lst) == 0);
}

/*}}}*/

// ft_list_push_front{{{

#define	TEST_LST(CODE, TST) \
	do { \
		TEST(CODE, TST); \
		ft_lst_clear(&l1); \
		ft_lst_clear(&l2); \
	} while (0)

void	list_pushfront_test(void)
{
	t_list	*l1 = 0;
	t_list	*l2 = 0;
	test_title("ft_list_push_front");
	test_description("\
Écrire la fonction ft_list_push_front qui ajoute au début de la liste un nouvel \
élément de type t_list. \n\
• Elle devra assigner data au paramètre fourni. \n\
• Elle mettra à jour, si nécéssaire, le pointeur sur le début de liste. \n\
• Elle devra être prototypée de la façon suivante : \n\
\t\e[35mvoid ft_list_push_front(t_list **begin_list, void *data); \n\
");
	TEST_LST(ft_list_push_front(&l1, "Hello"); ft_lst_push_front(&l2, "Hello");, lst_eq(l1, l2)); 
	TEST_LST(
			ft_list_push_front(&l1, "World"); ft_list_push_front(&l1, "Hola");
			ft_lst_push_front(&l2, "World");ft_lst_push_front(&l2, "Hola");
			, lst_eq(l1, l2)); 
	TEST_LST(ft_list_push_front(0, "Hello");, lst_eq(l1, 0)); 
}
/*}}}*/

// ft_list_sort{{{

void	list_sort_test()
{
	t_list	*l1;
	t_list	*l2;

	test_title("ft_list_sort");
	test_description("\
Écrire la fonction ft_list_sort qui trie par ordre croissant le contenu de la liste, \n\
en comparant deux maillons grâce à une fonction de comparaison de données des \n\
deux maillons. \n\
• Elle devra être prototypée de la façon suivante : \n\
\t\e[35mvoid ft_list_sort(t_list **begin_list, int (*cmp)()); \n\
");
	TEST_LST(
			l1 = lst_create_sample((char *[]){"hello", 0}); l2 = lst_create_sample((char *[]){"hello", 0});
			ft_list_sort(&l1, strcmp); ft_lst_sort(&l2, strcmp); lst_print(l1), lst_eq(l1, l2));
	TEST_LST(
			l1 = lst_create_sample((char *[]){"hello", "world", "mjacq", "2", "1", "3", 0});
			l2 = lst_create_sample((char *[]){"hello", "world", "mjacq", "2", "1", "3", 0});
			ft_list_sort(&l1, strcmp); ft_lst_sort(&l2, strcmp); lst_print(l1), lst_eq(l1, l2));
	TEST_LST(l1 = 0; l2 = 0; ft_list_sort(&l1, strcmp); ft_lst_sort(&l2, strcmp); lst_print(l1), lst_eq(l1, l2));
}

/*}}}*/

// ft_remove_if{{{

void	list_remove_if_test(void)
{
	t_list	*l1;
	t_list	*l2;

	test_title("ft_list_remove_if");
	test_description("\
• Écrire la fonction ft_list_remove_if qui efface de la liste tous les élements dont \n\
la donnée est \"égale\" à la donnée de référence. \n\
• Elle devra être prototypée de la façon suivante : \n\
\t\e[35mvoid ft_list_remove_if(t_list **begin_list, void *data_ref, int (*cmp)()); \n\
");

	TEST_LST(
			l1 = lst_create_sample((char *[]){"hello", "world", "mjacq", "2", "1", "3", 0});
			l2 = lst_create_sample((char *[]){"hello", "world", "mjacq", "2", "1", "3", 0});
			ft_list_remove_if(&l1, "hello", strcmp); ft_lst_remove_if(&l2, "hello", strcmp);
			lst_print(l1), lst_eq(l1, l2));
	TEST_LST(
			l1 = lst_create_sample((char *[]){"hello", "hello", "mjacq", "2", "1", "3", 0});
			l2 = lst_create_sample((char *[]){"hello", "hello", "mjacq", "2", "1", "3", 0});
			ft_list_remove_if(&l1, "hello", strcmp); ft_lst_remove_if(&l2, "hello", strcmp);
			lst_print(l1), lst_eq(l1, l2));
	TEST_LST(
			l1 = lst_create_sample((char *[]){"hello", "world", "mjacq", "2", "1", "3", 0});
			l2 = lst_create_sample((char *[]){"hello", "world", "mjacq", "2", "1", "3", 0});
			ft_list_remove_if(&l1, "mjacq", strcmp); ft_lst_remove_if(&l2, "mjacq", strcmp);
			lst_print(l1), lst_eq(l1, l2));
	TEST_LST(
			l1 = lst_create_sample((char *[]){"mjacq", "world", "mjacq", "mjacq", "1", "mjacq", 0});
			l2 = lst_create_sample((char *[]){"mjacq", "world", "mjacq", "mjacq", "1", "mjacq", 0});
			ft_list_remove_if(&l1, "mjacq", strcmp); ft_lst_remove_if(&l2, "mjacq", strcmp);
			lst_print(l1), lst_eq(l1, l2));
	TEST_LST(
			l1 = lst_create_sample((char *[]){"hello", "world", "mjacq", "2", "1", "3", 0});
			l2 = lst_create_sample((char *[]){"hello", "world", "mjacq", "2", "1", "3", 0});
			ft_list_remove_if(&l1, "ZZZ", strcmp); ft_lst_remove_if(&l2, "ZZZ", strcmp);
			lst_print(l1), lst_eq(l1, l2));
	TEST_LST(
			l1 = lst_create_sample((char *[]){"hello", 0});
			l2 = lst_create_sample((char *[]){"hello", 0});
			ft_list_remove_if(&l1, "hello", strcmp); ft_lst_remove_if(&l2, "hello", strcmp);
			lst_print(l1), lst_eq(l1, l2));
	TEST_LST(
			l1 = lst_create_sample((char *[]){0});
			l2 = lst_create_sample((char *[]){0});
			ft_list_remove_if(&l1, "hello", strcmp); ft_lst_remove_if(&l2, "hello", strcmp);
			lst_print(l1), lst_eq(l1, l2));
}

/*}}}*/

// ft_atoi_base{{{

void	atoi_base_test(void)
{
	char s[20];

	test_title("ft_atoi_base");
	test_description("\
• Écrire une fonction qui renvoie un nombre. Ce nombre est connu sous la forme d’une \
chaîne de caractères. \n\
• La chaîne de caractères exprime le nombre dans une base particulière passée en \
second paramètre. \n\
• La fonction doit gérer les nombres négatifs. \n\
• La fonction doit les signes comme man atoi. \n\
• Si un paramètre contient une erreur la fonction renvoie 0. Une erreur peut être : \n\
\t◦ str est une chaîne vide ; \n\
\t◦ la base est vide ou est de taille 1 ; \n\
\t◦ str contient des caractères qui ne sont pas dans la base et ne sont pas + ou - ; \n\
\t◦ la base contient deux fois le même caractère ; \n\
\t◦ la base contient les caractères + ou - ; \n\
\t◦ etc. \n\
• Elle devra être prototypée de la façon suivante : \n\
\t\e[35mint ft_atoi_base(char *str, char *base);\n\
");
	TEST(,ft_atoi_base(0, "0123") == 0);
	TEST(,ft_atoi_base("0123", 0) == 0);
	TEST(,ft_atoi_base("1", "01232") == 0);
	TEST(,ft_atoi_base("0", "0") == 0);
	TEST(,ft_atoi_base("1", "023") == 0);
	TEST(,ft_atoi_base("1", "01-") == 0);
	TEST(,ft_atoi_base("1", "01+") == 0);
	printf("%d\n", ft_atoi_base("0", "012"));
	TEST(,ft_atoi_base("0", "012") == 0);
	printf("%d\n", ft_atoi_base("1", "012"));
	TEST(,ft_atoi_base("1", "012") == 1);
	printf("%d\n", ft_atoi_base("2", "012"));
	TEST(,ft_atoi_base("2", "012") == 2);
	printf("%d\n", ft_atoi_base("12", "012"));
	TEST(,ft_atoi_base("12", "012") == 3 + 2);
	printf("%d\n", ft_atoi_base("-12", "012"));
	TEST(,ft_atoi_base("-12", "012") == - 3 - 2);
	TEST(,ft_atoi_base("0", "012") == 0);
	TEST(,ft_atoi_base("1011010", "01") == 0b1011010);
	TEST(,ft_atoi_base("ABC120F", "0123456789ABCDEF") == 0xABC120F);
	TEST(,ft_atoi_base("0001920", "0123456789") == 1920);
	TEST(sprintf(s, "%d", INT_MAX );
			printf("%d\n", ft_atoi_base(s, "0123456789"));,
			ft_atoi_base(s, "0123456789") == INT_MAX);
	TEST(sprintf(s, "%d", INT_MIN + 1 );,
			ft_atoi_base(s, "0123456789") == INT_MIN + 1);
	TEST(sprintf(s, "%d", INT_MIN );,
			ft_atoi_base(s, "0123456789") == INT_MIN);
	TEST(sprintf(s, "%ld", (long)INT_MAX + 1 );,
			ft_atoi_base(s, "0123456789") == 0);
	TEST(sprintf(s, "%ld", (long)INT_MIN - 1 );,
			ft_atoi_base(s, "0123456789") == 0);
	TEST(sprintf(s, "%ld", (long)INT_MAX + 1042 );,
			ft_atoi_base(s, "0123456789") == 0);
	TEST(sprintf(s, "%ld", (long)INT_MIN - 1042 );,
			ft_atoi_base(s, "0123456789") == 0);
}

/*}}}*/

#endif
/*}}}*/
int	main(void)
{
#ifndef BONUS
	test_exemple();
	strlen_test();
	strcpy_test();
	strcmp_test();
	write_test();
	read_test();
	strdup_test();
#endif
#ifdef BONUS
	test_title("\e[32mBonuses\e[0m");
	atoi_base_test();
	list_pushfront_test();
	list_size_test();
	list_sort_test();
	list_remove_if_test();
#endif
	return (0);
}

// vim: fdm=marker: foldlevelstart=0
