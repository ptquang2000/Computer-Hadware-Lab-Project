const authSwitchLinks = document.querySelectorAll('.switch');
const authModals = document.querySelectorAll('.auth .modal');
const authWrapper = document.querySelector('.auth');
const registerForm = document.querySelector('.register');
const loginForm = document.querySelector('.login');
const signOut = document.querySelector('.sign-out');

// toggle auth modals
authSwitchLinks.forEach(link => {
  link.addEventListener('click', () => {
    authModals.forEach(modal => modal.classList.toggle('active'));
  });
});

// register form
registerForm.addEventListener('submit', (e) => {
    e.preventDefault();
    
    const email = registerForm.email.value;
    const password = registerForm.password.value;
  
    firebase.auth().createUserWithEmailAndPassword(email, password)
      .then(user => {
        console.log('registered', user.user.uid);
        const username = email.split('@')[0]
        var newConfig = {
          'id': user.user.uid,
          'total': 0,
          'username': username,
        };
        var updates = {};
        updates['/users/' + username] = newConfig;
        firebase.database().ref().update(updates);
        registerForm.reset();
      })
      .catch(error => {
        registerForm.querySelector('.error').textContent = error.message;
      });
  });
  
  // login form
  loginForm.addEventListener('submit', (e) => {
    e.preventDefault();
    
    const email = loginForm.email.value;
    const password = loginForm.password.value;
  
    firebase.auth().signInWithEmailAndPassword(email, password)
      .then(user => {
        console.log('logged in', user);
        loginForm.reset();
      })
      .catch(error => {
        loginForm.querySelector('.error').textContent = error.message;
      });
  });
  
  // sign out
  signOut.addEventListener('click', () => {
    firebase.auth().signOut()
      .then(() => console.log('signed out'));
  });
  
   // auth listener
  firebase.auth().onAuthStateChanged(user => {
    if (user) {
      // const username = firebase.auth().currentUser.email.split('@')[0];
      // firebase.database().ref('users/').child(username).on('value', snapshot =>{
      //     setupList(snapshot.val());
      // });

      var app = new Vue({
        el: '#app',
        data: {
          configs: [],
        },
        methods: {
            deleteRequest(username, config_id){
            firebase.database().ref('users/' + username + '/' + config_id).remove();
          }
        },
        mounted() {
          let configs = [];
          const username = firebase.auth().currentUser.email.split('@')[0];
          firebase.database().ref('users/').child(username).on('value', snapshot =>{
              Object.entries(snapshot.val()).map(data => {
                if (!data[0].includes('total') && !data[0].includes('id') && !data[0].includes('username'))
                  configs.push({...data[1], id: data[0], username: snapshot.val().username});
              })
          });
          this.configs = configs;
        }
      });

      authWrapper.classList.remove('open');
      authModals.forEach(modal => modal.classList.remove('active'));
    } else {
      authWrapper.classList.add('open');
      authModals[0].classList.add('active');
    }
  });