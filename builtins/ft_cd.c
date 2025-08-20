/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aylaaouf <aylaaouf@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 19:42:28 by aylaaouf          #+#    #+#             */
/*   Updated: 2025/07/13 00:59:41 by aylaaouf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <errno.h>
#include <string.h>

char	*get_env_value_cd(t_env *env, char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

void	update_env_var(t_gc *gc, t_env *env, char *key, char *new_value)
{
	t_env	*temp;

	temp = env;
	while (temp)
	{
		if (ft_strcmp(temp->key, key) == 0)
		{
			temp->value = gc_strdup(gc, new_value);
			return ;
		}
		temp = temp->next;
	}
}

int	ft_helper(char **args)
{
	if (args[1] && args[2])
	{
		write(2, "minishell: cd: too many arguments\n", 34);
		g_last_exit_status = 1;
		return (1);
	}
	return (0);
}

char	*get_target_path(t_gc *gc, char **args, t_env *env)
{
	char	*home;

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
		return (get_env_value_cd(env, "HOME"));
	else if (ft_strcmp(args[1], "-") == 0)
		return (get_env_value_cd(env, "OLDPWD"));
	else if (args[1][0] == '~')
	{
		home = get_env_value_cd(env, "HOME");
		if (home)
			return (ft_strjoin(gc, home, args[1] + 1));
		else
			return (args[1]);
	}
	return (args[1]);
}

int	count_args(char **args)
{
	int	count;

	count = 0;
	if (!args)
		return (0);
	while (args[count])
		count++;
	return (count);
}

void	cd_too_many_args(void)
{
	write(2, "minishell: cd: too many arguments\n", 34);
}

char	*get_enviroment(t_env *env, char *key)
{
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

char	*resolve_cd_path(char **args, t_env *env)
{
	char	*path;

	path = args[1];
	if (!path || ft_strcmp(path, "~") == 0)
	{
		path = get_enviroment(env, "HOME");
		if (!path)
		{
			write(2, "minishell: cd: HOME not set\n", 28);
			return (NULL);
		}
	}
	else if (ft_strcmp(path, "-") == 0)
	{
		path = get_enviroment(env, "OLDPWD");
		if (!path)
		{
			write(2, "minishell: cd: OLDPWD not set\n", 30);
			return (NULL);
		}
		printf("%s\n", path);
	}
	return (path);
}

char	*get_current_directory(void)
{
	char	cwd[1024];
	char	*current_pwd;

	if (getcwd(cwd, sizeof(cwd)))
	{
		current_pwd = malloc(ft_strlen(cwd) + 1);
		if (!current_pwd)
		{
			perror("minishell: cd");
			return (NULL);
		}
		ft_strcpy(current_pwd, cwd);
		return (current_pwd);
	}
	else
	{
		perror("minishell: cd");
		return (NULL);
	}
}

int	change_directory(char *path, char *oldpwd)
{
	if (chdir(path) != 0)
	{
		g_last_exit_status = 1;
		write(2, "minishell: cd: ", 15);
		write(2, path, ft_strlen(path));
		write(2, ": ", 2);
		write(2, strerror(errno), ft_strlen(strerror(errno)));
		write(2, "\n", 1);
		if (oldpwd)
			free(oldpwd);
		return (1);
	}
	return (0);
}

void	update_pwd_vars(t_env *env, char *oldpwd, char *newpwd)
{
	t_env	*temp;

	temp = env;
	while (temp)
	{
		if (ft_strcmp(temp->key, "OLDPWD") == 0)
		{
			if (temp->value)
			{
				// In a real implementation, we'd need proper memory management
				// For now, we'll just assign the new values
				temp->value = oldpwd;
			}
		}
		else if (ft_strcmp(temp->key, "PWD") == 0)
		{
			if (temp->value)
			{
				temp->value = newpwd;
			}
		}
		temp = temp->next;
	}
}

int	builtin_cd(char **args, t_env *env)
{
	char	*path;
	char	*oldpwd;
	char	*newpwd;
	int		args_count;

	args_count = count_args(args);
	if (args_count > 2)
	{
		cd_too_many_args();
		return (1);
	}
	path = resolve_cd_path(args, env);
	if (!path)
		return (1);
	oldpwd = get_current_directory();
	if (!oldpwd)
		return (1);
	if (change_directory(path, oldpwd) != 0)
		return (1);
	newpwd = get_current_directory();
	if (!newpwd)
	{
		free(oldpwd);
		return (1);
	}
	update_pwd_vars(env, oldpwd, newpwd);
	// Note: In a proper implementation, we wouldn't free these here
	// since they're now referenced in the environment
	// free(oldpwd);
	// free(newpwd);
	return (0);
}

void	ft_cd(t_gc *gc, char **args, t_env *env)
{
	char	cwd[4096];
	char	*target;

	if (ft_helper(args))
		return ;
	getcwd(cwd, sizeof(cwd));
	target = get_target_path(gc, args, env);
	if (!target || chdir(target) != 0)
	{
		perror("minishell: cd");
		g_last_exit_status = 1;
		return ;
	}
	update_env_var(gc, env, "OLDPWD", cwd);
	getcwd(cwd, sizeof(cwd));
	update_env_var(gc, env, "PWD", cwd);
	g_last_exit_status = 0;
}
