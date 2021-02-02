use std::env;
use std::io::{stdin, stdout, Write};
use std::path::Path;
use std::process::{Child, Command, Stdio};

fn main() {
    let mut input = String::new();

    loop {
        print!("$ ");
        stdout().flush().ok();

        stdin().read_line(&mut input).unwrap();

        let mut commands = input.trim().split(" | ").peekable();
        let mut previous_command = None;

        while let Some(command) = commands.next() {
            let mut args = command.trim().split_whitespace();
            let command = args.next().unwrap();

            match command {
                "cd" => {
                    let new_dir = args.next().unwrap_or_else(|| "/");
                    let root = Path::new(new_dir);
                    if let Err(e) = env::set_current_dir(&root) {
                        eprintln!("{}", e);
                    }

                    previous_command = None;
                }

                "exit" => return,

                command => {
                    let stdin = previous_command.map_or(Stdio::inherit(), |output: Child| {
                        Stdio::from(output.stdout.unwrap())
                    });

                    let stdout = if commands.peek().is_some() {
                        Stdio::piped()
                    } else {
                        Stdio::inherit()
                    };

                    let output = Command::new(command)
                        .args(args)
                        .stdin(stdin)
                        .stdout(stdout)
                        .spawn();

                    match output {
                        Ok(output) => {
                            previous_command = Some(output);
                        }
                        Err(_e) => {
                            previous_command = None;
                            eprintln!("dango: command not found: {}", command);
                        }
                    };
                }
            }
        }

        if let Some(mut final_command) = previous_command {
            final_command.wait().ok();
        }
    }
}
