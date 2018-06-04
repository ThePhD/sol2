project('sol2', 'cpp')

# Expose standard dependency
sol2_dep = declare_dependency(
	include_directories: include_directories('.'),
)


# Check if we have python installed (required for creating single)
python = find_program('python3', required: false)

if not python.found()
	python = find_program('python', required: false)
endif

single_opt = get_option('single')

if not python.found() and single_opt
	error('Could not locate Python. Python is required when building a single header.')
endif


# Single header targets requested
if single_opt

	# Get sol header files
	cmd = run_command(python, 'list-headers.py')

	if cmd.returncode() != 0
		error('Could not list sol2 header files.')
	endif

	# Setup the single generation target
	sol2_single = custom_target('sol2_single', 
		output: 'sol2.hpp',
		input: cmd.stdout().strip().split('\n'),
		command: [ python, 'single.py', '--output', '@OUTPUT@' ],
	)

	# Create the dependency
	sol2_single_dep = declare_dependency(
		sources: [ sol2_single ],
	)
endif