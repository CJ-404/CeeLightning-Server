
const apiUrl = 'http://localhost:8080';

// Function to fetch users from the backend
function fetchUsers() {
    fetch(`${apiUrl}/users`)
        .then(response => response.json())
        .then(users => {

            console.log("users: ", users);
            const userList = document.getElementById('user-list');
            userList.innerHTML = '';

            users.forEach(user => {
                const userItem = document.createElement('div');
                userItem.className = 'user-item';
                userItem.innerHTML = `
                    <span>${user.name} (${user.email})</span>
                    <button onclick="deleteUser('${user.id}')">Delete</button>
                `;
                userList.appendChild(userItem);
            });
        });
}

// Function to delete a user
function deleteUser(id) {
    fetch(`${apiUrl}/user?id=${id}`, {
        method: 'DELETE'
    })
    .then((response) => {
        
        if( response.status == "204")
        {
            fetchUsers(); // Refresh the user list after deletion
        } else{
            alert(response.status + " , " + response.statusText)
        }
    });
}

// Function to add a user
function addUser() {
    const name = document.getElementById('name').value;
    const email = document.getElementById('email').value;

    const userData = { name, email };
    let body = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(userData)
    }
    fetch(`${apiUrl}/users`, body)
    .then((response) => {
        // console.log(response.status, response.statusText);
        if( response.status == "201")
        {
            fetchUsers(); // Refresh the user list after adding
            document.getElementById('name').value = '';
            document.getElementById('email').value = '';
        } else{
            alert(response.status + " , " + response.statusText)
        }
    });
}

// Fetch users when the page loads
window.onload = fetchUsers;