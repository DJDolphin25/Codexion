#include <codexion.h>

int main(int ac, char **av)
{
    t_args args;

    if (ac == 9)
    {
        if (parse_args(ac, av, &args) == 0)
            return (1);
    }
    else
    {
        fprintf(stderr, "%s", "Number of arguments different from 9.\n");
        return (1);
    }
    return (0);
}
