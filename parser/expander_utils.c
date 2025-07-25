/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aylaaouf <aylaaouf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 12:05:21 by ayelasef          #+#    #+#             */
/*   Updated: 2025/07/15 17:13:39 by ayelasef         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_env_value(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return ("");
}

char	*extract_var_name(t_gc *gc, char *str, size_t *i)
{
	size_t	start;

	start = *i;
	if (str[*i] == '?')
	{
		(*i)++;
		return (gc_strndup(gc, "?", 1));
	}
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	return (gc_strndup(gc, &str[start], *i - start));
}

char	**split_words(t_gc *gc, char *str)
{
	char	**result;
	size_t	count;
	char	*token;
	char	*copy;
	char	*saveptr;

	if (!str || !*str)
		return (NULL);
	result = NULL;
	count = 0;
	copy = gc_strdup(gc, str);
	token = strtok_r(copy, " \t\n", &saveptr);
	while (token)
	{
		result = gc_realloc(gc, result, sizeof(char *) * count, sizeof(char *)
				* (count + 2));
		result[count++] = gc_strdup(gc, token);
		token = strtok_r(NULL, " \t\n", &saveptr);
	}
	if (result)
		result[count] = NULL;
	return (result);
}

int	needs_word_splitting(char *str)
{
	return (str && (strchr(str, ' ') || strchr(str, '\t')));
}

t_token	*create_split_tokens(t_gc *gc, char **split, t_token *next,
		bool has_space_before)
{
	t_token	*first;
	t_token	*current;
	int		i;

	if (!split || !split[0])
		return (next);
	first = new_token(TOKEN_WORD, split[0], gc, has_space_before);
	current = first;
	i = 1;
	while (split[i])
	{
		current->next = new_token(TOKEN_WORD, split[i], gc, true);
		current = current->next;
		i++;
	}
	current->next = next;
	return (first);
}
