# Student Submission Program

This C++ utility provides a secure way for students to submit assignments into a faculty-owned directory without being able to see or overwrite other students' submissions.

It is intended for use on a shared NFS-mounted directory (e.g., `/nfscommon/common/`) that is accessible by all users. Such a directory is referred as `<common>` below.

## Features

- **Secure submission**: Students cannot list, read, or overwrite files in the faculty submission directory.
- **Automatic per-student folder creation**: Each student’s submissions are stored in their own directory, owned by the instructor.
- **Read-only submissions**: Once submitted, files are set to `0444` (read-only).
- **Optional listing**: Students can list the files they personally submitted (`-l` option).

## Directory Structure

The submissions are maintained in the `submissions` directory structured as:

`<common>/<faculty_username>/submissions/<student_username>/`

Where:
- `<faculty_username>` is the username of the instructor who owns the top-level directory.
- `<student_username>` is automatically detected from the logged-in student.

## 1. Faculty Setup

1. Ensure `<common>/<faculty_username>`  is present. If not, create the folder or ask admin to create it. Give permissions to `<faculty_username>` folder.
```
    chmod 755 <common>/<faculty_username>
```
Verify that `<faculty_username>` is owned by the faculty.

2. Create folder submissions in `<common>/<faculty_username>`
```
    cd <common>/<faculty_username>
    mkdir submissions
    chmod 700 submissions
```

3. Complile the program in submit_asgn folder
```
    g++ -Wall submit.cpp -o submit
```

4. copy the `submit` executable in faculty folder
```
    cp submit <common>/<faculty_username>/
```

5. Set permissions for the executable
```
    cd <common>/<faculty_username>
    chmod 4755 submit
```

## 2. Student Submissions
1. Student can submit any number of files or directory which can be passed as parameters to the the submit program.
```
    <common>/<faculty_username>/submit  file1 dir1 ...
```

The submitted files cannot be overwritten or deleted. For resubmission, give a new name, e.g. `file.v1.py` and resubmit. The file/dir names in the above command can be relative or absolute.

2. Students can list the files they have submitted by running:
```
    submit -l
```
