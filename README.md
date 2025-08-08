# Student Submission (submit.cpp)

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

---

# Marks Viewer (marks.cpp)

This program allows a logged-in student to securely view **only their own marks** from a faculty-maintained evaluation database.
It automatically looks up the student’s identity, matches it to their registered email ID, and prints their record in a readable format.
It also logs every lookup for auditing purposes.

## Features

- Automatically detects the **logged-in username**.
- Maps usernames to email IDs using an alias file (`alias.csv`).
- Looks up marks in `eval.csv` by email ID.
- Pretty-prints the student’s marks and related fields.
- Records every query in `log.csv` with timestamp, username, and email ID.
- Prevents students from seeing other students’ marks.

## Directory Structure

The program expects its data files to be located in: `/home/<userid>/data_dir/`, where `<userid>` is the faculty/instructor account that owns and maintains the files.

Required files:

1. **`eval.csv`** — main evaluation data, with header row. Must have **email ID in column 2**.

```
rollno,emailid,marks,remarks
21CS001,alice,95,Excellent
21CS002,bob,88,Good work
```

2. **`alias.csv`** — maps system usernames to email IDs, if required. Must have header row.

```
userid,emailid
alicera,alice
bobin,bob
```

3. **`log.csv`** — will be appended to by the script whenever a student checks marks. Example log entry:
```
08 Aug 2025 14:35:20 alicera alice
```

## 1. Faculty Setup

### Step 1 — Create data directory
```
mkdir -p /home/<faculty_user>/data_dir
```

### Step 2 — Place CSV files
Create eval.csv and alias.csv in that directory.  Ensure they have the correct format as shown above.

### Step 3 — Set permissions

Faculty user owns the directory and files
```
chmod -R 600 /home/<faculty_user>/data_dir
```

2. Compilation
```
# edit marks.cpp and set appropriate value of the variable base_dir
g++ -std=c++17 marks.cpp -o view_marks
```

3. Installation & SUID Setup:
To allow the program to read faculty-owned files securely.
```
chmod 4755 view_marks
# Move binary to a shared location
mv view_marks /nfscommon/common/<faculty_user>/
```

## 2. Usage (Students)
Students simply run:
```
/nfscommon/command/<faculty_user>/view_marks
```

Example output:
```
User id : alicera
rollno  : 21CS001
emailid : alice
marks   : 95
remarks : Excellent
```
