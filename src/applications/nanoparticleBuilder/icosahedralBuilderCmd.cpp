/*
  File autogenerated by gengetopt version 2.22.4
  generated with the following command:
  gengetopt --file-name=icosahedralBuilderCmd --unamed-opts 

  The developers of gengetopt consider the fixed text that goes in all
  gengetopt output files to be in the public domain:
  we make no copyright claims on it.
*/

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FIX_UNUSED
#define FIX_UNUSED(X) (void) (X) /* avoid warnings for unused params */
#endif

#include <getopt.h>

#include "icosahedralBuilderCmd.h"

const char *gengetopt_args_info_purpose = "Builds icosahedral nanoparticles and outputs an OpenMD startup file";

const char *gengetopt_args_info_usage = "Usage: icosahedralBuilder [OPTIONS]... [FILES]...";

const char *gengetopt_args_info_description = "";

const char *gengetopt_args_info_help[] = {
  "  -h, --help                    Print help and exit",
  "  -V, --version                 Print version and exit",
  "  -o, --output=STRING           Output file name",
  "  -n, --shells=INT              Nanoparticle shells",
  "  -d, --latticeConstant=DOUBLE  Lattice spacing in Angstroms for cubic lattice.",
  "  -c, --columnAtoms=INT         Number of atoms along central column \n                                  (Decahedron only)",
  "  -t, --twinAtoms=INT           Number of atoms along twin boundary (Decahedron \n                                  only)",
  "  -p, --truncatedPlanes=INT     Number of truncated planes (Curling-stone \n                                  Decahedron only)",
  "\n Group: clusterShape\n   a cluster shape is required",
  "      --ico                     Create an Icosahedral cluster",
  "      --deca                    Create a regualar Decahedral cluster",
  "      --ino                     Create an Ino Decahedral cluster",
  "      --marks                   Create a Marks Decahedral cluster",
  "      --stone                   Create a Curling-stone Decahedral cluster",
    0
};

typedef enum {ARG_NO
  , ARG_STRING
  , ARG_INT
  , ARG_DOUBLE
} cmdline_parser_arg_type;

static
void clear_given (struct gengetopt_args_info *args_info);
static
void clear_args (struct gengetopt_args_info *args_info);

static int
cmdline_parser_internal (int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error);

static int
cmdline_parser_required2 (struct gengetopt_args_info *args_info, const char *prog_name, const char *additional_error);

static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct gengetopt_args_info *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->output_given = 0 ;
  args_info->shells_given = 0 ;
  args_info->latticeConstant_given = 0 ;
  args_info->columnAtoms_given = 0 ;
  args_info->twinAtoms_given = 0 ;
  args_info->truncatedPlanes_given = 0 ;
  args_info->ico_given = 0 ;
  args_info->deca_given = 0 ;
  args_info->ino_given = 0 ;
  args_info->marks_given = 0 ;
  args_info->stone_given = 0 ;
  args_info->clusterShape_group_counter = 0 ;
}

static
void clear_args (struct gengetopt_args_info *args_info)
{
  FIX_UNUSED (args_info);
  args_info->output_arg = NULL;
  args_info->output_orig = NULL;
  args_info->shells_orig = NULL;
  args_info->latticeConstant_orig = NULL;
  args_info->columnAtoms_orig = NULL;
  args_info->twinAtoms_orig = NULL;
  args_info->truncatedPlanes_orig = NULL;
  
}

static
void init_args_info(struct gengetopt_args_info *args_info)
{


  args_info->help_help = gengetopt_args_info_help[0] ;
  args_info->version_help = gengetopt_args_info_help[1] ;
  args_info->output_help = gengetopt_args_info_help[2] ;
  args_info->shells_help = gengetopt_args_info_help[3] ;
  args_info->latticeConstant_help = gengetopt_args_info_help[4] ;
  args_info->columnAtoms_help = gengetopt_args_info_help[5] ;
  args_info->twinAtoms_help = gengetopt_args_info_help[6] ;
  args_info->truncatedPlanes_help = gengetopt_args_info_help[7] ;
  args_info->ico_help = gengetopt_args_info_help[9] ;
  args_info->deca_help = gengetopt_args_info_help[10] ;
  args_info->ino_help = gengetopt_args_info_help[11] ;
  args_info->marks_help = gengetopt_args_info_help[12] ;
  args_info->stone_help = gengetopt_args_info_help[13] ;
  
}

void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n",
     (strlen(CMDLINE_PARSER_PACKAGE_NAME) ? CMDLINE_PARSER_PACKAGE_NAME : CMDLINE_PARSER_PACKAGE),
     CMDLINE_PARSER_VERSION);
}

static void print_help_common(void) {
  cmdline_parser_print_version ();

  if (strlen(gengetopt_args_info_purpose) > 0)
    printf("\n%s\n", gengetopt_args_info_purpose);

  if (strlen(gengetopt_args_info_usage) > 0)
    printf("\n%s\n", gengetopt_args_info_usage);

  printf("\n");

  if (strlen(gengetopt_args_info_description) > 0)
    printf("%s\n\n", gengetopt_args_info_description);
}

void
cmdline_parser_print_help (void)
{
  int i = 0;
  print_help_common();
  while (gengetopt_args_info_help[i])
    printf("%s\n", gengetopt_args_info_help[i++]);
}

void
cmdline_parser_init (struct gengetopt_args_info *args_info)
{
  clear_given (args_info);
  clear_args (args_info);
  init_args_info (args_info);

  args_info->inputs = 0;
  args_info->inputs_num = 0;
}

void
cmdline_parser_params_init(struct cmdline_parser_params *params)
{
  if (params)
    { 
      params->override = 0;
      params->initialize = 1;
      params->check_required = 1;
      params->check_ambiguity = 0;
      params->print_errors = 1;
    }
}

struct cmdline_parser_params *
cmdline_parser_params_create(void)
{
  struct cmdline_parser_params *params = 
    (struct cmdline_parser_params *)malloc(sizeof(struct cmdline_parser_params));
  cmdline_parser_params_init(params);  
  return params;
}

static void
free_string_field (char **s)
{
  if (*s)
    {
      free (*s);
      *s = 0;
    }
}


static void
cmdline_parser_release (struct gengetopt_args_info *args_info)
{
  unsigned int i;
  free_string_field (&(args_info->output_arg));
  free_string_field (&(args_info->output_orig));
  free_string_field (&(args_info->shells_orig));
  free_string_field (&(args_info->latticeConstant_orig));
  free_string_field (&(args_info->columnAtoms_orig));
  free_string_field (&(args_info->twinAtoms_orig));
  free_string_field (&(args_info->truncatedPlanes_orig));
  
  
  for (i = 0; i < args_info->inputs_num; ++i)
    free (args_info->inputs [i]);

  if (args_info->inputs_num)
    free (args_info->inputs);

  clear_given (args_info);
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  FIX_UNUSED (values);
  if (arg) {
    fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}


int
cmdline_parser_dump(FILE *outfile, struct gengetopt_args_info *args_info)
{
  int i = 0;

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot dump options to stream\n", CMDLINE_PARSER_PACKAGE);
      return EXIT_FAILURE;
    }

  if (args_info->help_given)
    write_into_file(outfile, "help", 0, 0 );
  if (args_info->version_given)
    write_into_file(outfile, "version", 0, 0 );
  if (args_info->output_given)
    write_into_file(outfile, "output", args_info->output_orig, 0);
  if (args_info->shells_given)
    write_into_file(outfile, "shells", args_info->shells_orig, 0);
  if (args_info->latticeConstant_given)
    write_into_file(outfile, "latticeConstant", args_info->latticeConstant_orig, 0);
  if (args_info->columnAtoms_given)
    write_into_file(outfile, "columnAtoms", args_info->columnAtoms_orig, 0);
  if (args_info->twinAtoms_given)
    write_into_file(outfile, "twinAtoms", args_info->twinAtoms_orig, 0);
  if (args_info->truncatedPlanes_given)
    write_into_file(outfile, "truncatedPlanes", args_info->truncatedPlanes_orig, 0);
  if (args_info->ico_given)
    write_into_file(outfile, "ico", 0, 0 );
  if (args_info->deca_given)
    write_into_file(outfile, "deca", 0, 0 );
  if (args_info->ino_given)
    write_into_file(outfile, "ino", 0, 0 );
  if (args_info->marks_given)
    write_into_file(outfile, "marks", 0, 0 );
  if (args_info->stone_given)
    write_into_file(outfile, "stone", 0, 0 );
  

  i = EXIT_SUCCESS;
  return i;
}

int
cmdline_parser_file_save(const char *filename, struct gengetopt_args_info *args_info)
{
  FILE *outfile;
  int i = 0;

  outfile = fopen(filename, "w");

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot open file for writing: %s\n", CMDLINE_PARSER_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  i = cmdline_parser_dump(outfile, args_info);
  fclose (outfile);

  return i;
}

void
cmdline_parser_free (struct gengetopt_args_info *args_info)
{
  cmdline_parser_release (args_info);
}

/** @brief replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = 0;
  if (!s)
    return result;

  result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

static void
reset_group_clusterShape(struct gengetopt_args_info *args_info)
{
  if (! args_info->clusterShape_group_counter)
    return;
  
  args_info->ico_given = 0 ;
  args_info->deca_given = 0 ;
  args_info->ino_given = 0 ;
  args_info->marks_given = 0 ;
  args_info->stone_given = 0 ;

  args_info->clusterShape_group_counter = 0;
}

int
cmdline_parser (int argc, char **argv, struct gengetopt_args_info *args_info)
{
  return cmdline_parser2 (argc, argv, args_info, 0, 1, 1);
}

int
cmdline_parser_ext (int argc, char **argv, struct gengetopt_args_info *args_info,
                   struct cmdline_parser_params *params)
{
  int result;
  result = cmdline_parser_internal (argc, argv, args_info, params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser2 (int argc, char **argv, struct gengetopt_args_info *args_info, int override, int initialize, int check_required)
{
  int result;
  struct cmdline_parser_params params;
  
  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  result = cmdline_parser_internal (argc, argv, args_info, &params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required (struct gengetopt_args_info *args_info, const char *prog_name)
{
  int result = EXIT_SUCCESS;

  if (cmdline_parser_required2(args_info, prog_name, 0) > 0)
    result = EXIT_FAILURE;

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required2 (struct gengetopt_args_info *args_info, const char *prog_name, const char *additional_error)
{
  int error = 0;
  FIX_UNUSED (additional_error);

  /* checks for required options */
  if (! args_info->output_given)
    {
      fprintf (stderr, "%s: '--output' ('-o') option required%s\n", prog_name, (additional_error ? additional_error : ""));
      error = 1;
    }
  
  if (! args_info->shells_given)
    {
      fprintf (stderr, "%s: '--shells' ('-n') option required%s\n", prog_name, (additional_error ? additional_error : ""));
      error = 1;
    }
  
  if (args_info->clusterShape_group_counter == 0)
    {
      fprintf (stderr, "%s: %d options of group clusterShape were given. One is required%s.\n", prog_name, args_info->clusterShape_group_counter, (additional_error ? additional_error : ""));
      error = 1;
    }
  

  /* checks for dependences among options */

  return error;
}


static char *package_name = 0;

/**
 * @brief updates an option
 * @param field the generic pointer to the field to update
 * @param orig_field the pointer to the orig field
 * @param field_given the pointer to the number of occurrence of this option
 * @param prev_given the pointer to the number of occurrence already seen
 * @param value the argument for this option (if null no arg was specified)
 * @param possible_values the possible values for this option (if specified)
 * @param default_value the default value (in case the option only accepts fixed values)
 * @param arg_type the type of this option
 * @param check_ambiguity @see cmdline_parser_params.check_ambiguity
 * @param override @see cmdline_parser_params.override
 * @param no_free whether to free a possible previous value
 * @param multiple_option whether this is a multiple option
 * @param long_opt the corresponding long option
 * @param short_opt the corresponding short option (or '-' if none)
 * @param additional_error possible further error specification
 */
static
int update_arg(void *field, char **orig_field,
               unsigned int *field_given, unsigned int *prev_given, 
               char *value, const char *possible_values[],
               const char *default_value,
               cmdline_parser_arg_type arg_type,
               int check_ambiguity, int override,
               int no_free, int multiple_option,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  char *stop_char = 0;
  const char *val = value;
  int found;
  char **string_field;
  FIX_UNUSED (field);

  stop_char = 0;
  found = 0;

  if (!multiple_option && prev_given && (*prev_given || (check_ambiguity && *field_given)))
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: `--%s' (`-%c') option given more than once%s\n", 
               package_name, long_opt, short_opt,
               (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: `--%s' option given more than once%s\n", 
               package_name, long_opt,
               (additional_error ? additional_error : ""));
      return 1; /* failure */
    }

  FIX_UNUSED (default_value);
    
  if (field_given && *field_given && ! override)
    return 0;
  if (prev_given)
    (*prev_given)++;
  if (field_given)
    (*field_given)++;
  if (possible_values)
    val = possible_values[found];

  switch(arg_type) {
  case ARG_INT:
    if (val) *((int *)field) = strtol (val, &stop_char, 0);
    break;
  case ARG_DOUBLE:
    if (val) *((double *)field) = strtod (val, &stop_char);
    break;
  case ARG_STRING:
    if (val) {
      string_field = (char **)field;
      if (!no_free && *string_field)
        free (*string_field); /* free previous string */
      *string_field = gengetopt_strdup (val);
    }
    break;
  default:
    break;
  };

  /* check numeric conversion */
  switch(arg_type) {
  case ARG_INT:
  case ARG_DOUBLE:
    if (val && !(stop_char && *stop_char == '\0')) {
      fprintf(stderr, "%s: invalid numeric value: %s\n", package_name, val);
      return 1; /* failure */
    }
    break;
  default:
    ;
  };

  /* store the original value */
  switch(arg_type) {
  case ARG_NO:
    break;
  default:
    if (value && orig_field) {
      if (no_free) {
        *orig_field = value;
      } else {
        if (*orig_field)
          free (*orig_field); /* free previous string */
        *orig_field = gengetopt_strdup (value);
      }
    }
  };

  return 0; /* OK */
}


int
cmdline_parser_internal (
  int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error)
{
  int c;	/* Character of the parsed option.  */

  int error = 0;
  struct gengetopt_args_info local_args_info;
  
  int override;
  int initialize;
  int check_required;
  int check_ambiguity;
  
  package_name = argv[0];
  
  override = params->override;
  initialize = params->initialize;
  check_required = params->check_required;
  check_ambiguity = params->check_ambiguity;

  if (initialize)
    cmdline_parser_init (args_info);

  cmdline_parser_init (&local_args_info);

  optarg = 0;
  optind = 0;
  opterr = params->print_errors;
  optopt = '?';

  while (1)
    {
      int option_index = 0;

      static struct option long_options[] = {
        { "help",	0, NULL, 'h' },
        { "version",	0, NULL, 'V' },
        { "output",	1, NULL, 'o' },
        { "shells",	1, NULL, 'n' },
        { "latticeConstant",	1, NULL, 'd' },
        { "columnAtoms",	1, NULL, 'c' },
        { "twinAtoms",	1, NULL, 't' },
        { "truncatedPlanes",	1, NULL, 'p' },
        { "ico",	0, NULL, 0 },
        { "deca",	0, NULL, 0 },
        { "ino",	0, NULL, 0 },
        { "marks",	0, NULL, 0 },
        { "stone",	0, NULL, 0 },
        { 0,  0, 0, 0 }
      };

      c = getopt_long (argc, argv, "hVo:n:d:c:t:p:", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':	/* Print help and exit.  */
          cmdline_parser_print_help ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'V':	/* Print version and exit.  */
          cmdline_parser_print_version ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'o':	/* Output file name.  */
        
        
          if (update_arg( (void *)&(args_info->output_arg), 
               &(args_info->output_orig), &(args_info->output_given),
              &(local_args_info.output_given), optarg, 0, 0, ARG_STRING,
              check_ambiguity, override, 0, 0,
              "output", 'o',
              additional_error))
            goto failure;
        
          break;
        case 'n':	/* Nanoparticle shells.  */
        
        
          if (update_arg( (void *)&(args_info->shells_arg), 
               &(args_info->shells_orig), &(args_info->shells_given),
              &(local_args_info.shells_given), optarg, 0, 0, ARG_INT,
              check_ambiguity, override, 0, 0,
              "shells", 'n',
              additional_error))
            goto failure;
        
          break;
        case 'd':	/* Lattice spacing in Angstroms for cubic lattice..  */
        
        
          if (update_arg( (void *)&(args_info->latticeConstant_arg), 
               &(args_info->latticeConstant_orig), &(args_info->latticeConstant_given),
              &(local_args_info.latticeConstant_given), optarg, 0, 0, ARG_DOUBLE,
              check_ambiguity, override, 0, 0,
              "latticeConstant", 'd',
              additional_error))
            goto failure;
        
          break;
        case 'c':	/* Number of atoms along central column (Decahedron only).  */
        
        
          if (update_arg( (void *)&(args_info->columnAtoms_arg), 
               &(args_info->columnAtoms_orig), &(args_info->columnAtoms_given),
              &(local_args_info.columnAtoms_given), optarg, 0, 0, ARG_INT,
              check_ambiguity, override, 0, 0,
              "columnAtoms", 'c',
              additional_error))
            goto failure;
        
          break;
        case 't':	/* Number of atoms along twin boundary (Decahedron only).  */
        
        
          if (update_arg( (void *)&(args_info->twinAtoms_arg), 
               &(args_info->twinAtoms_orig), &(args_info->twinAtoms_given),
              &(local_args_info.twinAtoms_given), optarg, 0, 0, ARG_INT,
              check_ambiguity, override, 0, 0,
              "twinAtoms", 't',
              additional_error))
            goto failure;
        
          break;
        case 'p':	/* Number of truncated planes (Curling-stone Decahedron only).  */
        
        
          if (update_arg( (void *)&(args_info->truncatedPlanes_arg), 
               &(args_info->truncatedPlanes_orig), &(args_info->truncatedPlanes_given),
              &(local_args_info.truncatedPlanes_given), optarg, 0, 0, ARG_INT,
              check_ambiguity, override, 0, 0,
              "truncatedPlanes", 'p',
              additional_error))
            goto failure;
        
          break;

        case 0:	/* Long option with no short option */
          /* Create an Icosahedral cluster.  */
          if (strcmp (long_options[option_index].name, "ico") == 0)
          {
          
            if (args_info->clusterShape_group_counter && override)
              reset_group_clusterShape (args_info);
            args_info->clusterShape_group_counter += 1;
          
            if (update_arg( 0 , 
                 0 , &(args_info->ico_given),
                &(local_args_info.ico_given), optarg, 0, 0, ARG_NO,
                check_ambiguity, override, 0, 0,
                "ico", '-',
                additional_error))
              goto failure;
          
          }
          /* Create a regualar Decahedral cluster.  */
          else if (strcmp (long_options[option_index].name, "deca") == 0)
          {
          
            if (args_info->clusterShape_group_counter && override)
              reset_group_clusterShape (args_info);
            args_info->clusterShape_group_counter += 1;
          
            if (update_arg( 0 , 
                 0 , &(args_info->deca_given),
                &(local_args_info.deca_given), optarg, 0, 0, ARG_NO,
                check_ambiguity, override, 0, 0,
                "deca", '-',
                additional_error))
              goto failure;
          
          }
          /* Create an Ino Decahedral cluster.  */
          else if (strcmp (long_options[option_index].name, "ino") == 0)
          {
          
            if (args_info->clusterShape_group_counter && override)
              reset_group_clusterShape (args_info);
            args_info->clusterShape_group_counter += 1;
          
            if (update_arg( 0 , 
                 0 , &(args_info->ino_given),
                &(local_args_info.ino_given), optarg, 0, 0, ARG_NO,
                check_ambiguity, override, 0, 0,
                "ino", '-',
                additional_error))
              goto failure;
          
          }
          /* Create a Marks Decahedral cluster.  */
          else if (strcmp (long_options[option_index].name, "marks") == 0)
          {
          
            if (args_info->clusterShape_group_counter && override)
              reset_group_clusterShape (args_info);
            args_info->clusterShape_group_counter += 1;
          
            if (update_arg( 0 , 
                 0 , &(args_info->marks_given),
                &(local_args_info.marks_given), optarg, 0, 0, ARG_NO,
                check_ambiguity, override, 0, 0,
                "marks", '-',
                additional_error))
              goto failure;
          
          }
          /* Create a Curling-stone Decahedral cluster.  */
          else if (strcmp (long_options[option_index].name, "stone") == 0)
          {
          
            if (args_info->clusterShape_group_counter && override)
              reset_group_clusterShape (args_info);
            args_info->clusterShape_group_counter += 1;
          
            if (update_arg( 0 , 
                 0 , &(args_info->stone_given),
                &(local_args_info.stone_given), optarg, 0, 0, ARG_NO,
                check_ambiguity, override, 0, 0,
                "stone", '-',
                additional_error))
              goto failure;
          
          }
          
          break;
        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          goto failure;

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c%s\n", CMDLINE_PARSER_PACKAGE, c, (additional_error ? additional_error : ""));
          abort ();
        } /* switch */
    } /* while */

  if (args_info->clusterShape_group_counter > 1)
    {
      fprintf (stderr, "%s: %d options of group clusterShape were given. One is required%s.\n", argv[0], args_info->clusterShape_group_counter, (additional_error ? additional_error : ""));
      error = 1;
    }
  


  if (check_required)
    {
      error += cmdline_parser_required2 (args_info, argv[0], additional_error);
    }

  cmdline_parser_release (&local_args_info);

  if ( error )
    return (EXIT_FAILURE);

  if (optind < argc)
    {
      int i = 0 ;
      int found_prog_name = 0;
      /* whether program name, i.e., argv[0], is in the remaining args
         (this may happen with some implementations of getopt,
          but surely not with the one included by gengetopt) */

      i = optind;
      while (i < argc)
        if (argv[i++] == argv[0]) {
          found_prog_name = 1;
          break;
        }
      i = 0;

      args_info->inputs_num = argc - optind - found_prog_name;
      args_info->inputs =
        (char **)(malloc ((args_info->inputs_num)*sizeof(char *))) ;
      while (optind < argc)
        if (argv[optind++] != argv[0])
          args_info->inputs[ i++ ] = gengetopt_strdup (argv[optind-1]) ;
    }

  return 0;

failure:
  
  cmdline_parser_release (&local_args_info);
  return (EXIT_FAILURE);
}
