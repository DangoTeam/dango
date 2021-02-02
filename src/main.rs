use std::io::{stdin, Write, stdout};
use std::process::{ Command, Stdio, Child };
use std::path::Path;
use std::env;
use users::{get_current_uid, get_user_by_uid}; 

fn main() {

    let user = get_user_by_uid(get_current_uid()).unwrap(); 
    
    loop {
        print!("{} ~ ", user.name().to_string_lossy());
        stdout().flush().ok();

        let mut input = String::new();
        stdin().read_line(&mut input).unwrap();

        let mut commands = input.trim().split(" | ").peekable();
        let mut previous_command = None;

        while let Some(command) = commands.next() {
            let mut args = command.trim().split_whitespace();
            let command = args.next().unwrap();

            match command {
                "cd" => {
                    let new_dir = args.next().unwrap_or("/");
                    let root = Path::new(new_dir);
                    if let Err(_e) = env::set_current_dir(&root) {
                        eprintln!("cd: no such file or directory: {}", new_dir);
                    }

                    previous_command = None;
                },

                "exit" => return,

                command => {
                    let stdin = previous_command
                        .map_or(
                            Stdio::inherit(),
                            |output: Child| Stdio::from(output.stdout.unwrap())
                        );

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
                        Ok(output) => { previous_command = Some(output); },
                        Err(_e) => {
                            previous_command = None;
                            eprintln!("dango: command not found: {}", command);
                        },
                    };
                }
            }
        }

        if let Some(mut final_command) = previous_command {
            final_command.wait().ok();
        }
    }
}