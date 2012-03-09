/*
 * Copyright 2007 Racemi Inc
 *
 * This file was copied from fusexmp.c in the fuse examples.  I've
 * removed its copyright because the only thing left from that is the
 * order of things and since those things have to exist in a fuse
 * module anyway it doesn't seem copyrightable.
 */

#include <unistd.h>
#include <sys/types.h>

#include <rregadmin/fuse/ops.h>
#include <rregadmin/fuse/log.h>


static struct fuse_opt rrafs_opts[] = {
    { "regdir=%s",  offsetof(struct rrafs_data, regdir), 0 },
    { "--regdir=%s", offsetof(struct rrafs_data, regdir), 0 },

    FUSE_OPT_END
};

int main(int argc, char *argv[])
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    int res;

    struct rrafs_data *data = rrafs_get_rrafs_data();
    data->mounter_uid = getuid();
    data->mounter_gid = getgid();

    if (fuse_opt_parse(&args, data, rrafs_opts, NULL) == -1)
    {
        rra_warning("Error parsing arguments");
        exit(1);
    }

    if (data->regdir == NULL)
    {
        rra_warning("Didn't get regdir");
        exit(1);
    }

    data->state = rra_cli_state_new_from_win_dir(data->regdir);

    res = fuse_main(args.argc, args.argv, rrafs_get_operations(), NULL);

    if (rra_cli_state_free(data->state))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
