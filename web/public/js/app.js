const configModal = document.querySelector('.new-config');
const configLink = document.querySelector('.add-config');
const configForm = document.querySelector('.new-config form');

configLink.addEventListener('click', ()=>{
    configModal.classList.add('open')
})

configModal.addEventListener('click', (e)=>{
    if (e.target.classList.contains('new-config')){
        configModal.classList.remove('open')
    }
})

configForm.addEventListener('submit', (e)=>{
    e.preventDefault();
    
    const username = firebase.auth().currentUser.email.split('@')[0];
    firebase.database().ref('users/' + username).child('numofconfig').get().then(function(snapshot) {
        if (snapshot.exists()) {

            var confignum = snapshot.val();
            var newConfig = {
                '0': configForm.month.value,
                '1': configForm.date.value,
                '2': configForm.year.value,
                '3': configForm.start.value,
                '4': configForm.stop.value,
            };
            var updates = {};
            var newPostKey = 'config' + confignum.toString();
            updates['/users/' + username + '/' + newPostKey] = newConfig;
            updates['/users/' + username + '/numofconfig'] = confignum + 1;    
        
            firebase.database().ref().update(updates);
            configForm.reset();
            configModal.classList.remove('open');

        }
        else {
            console.log("No data available");
        }
        }).catch(function(error) {
            console.error(error);
    });
    
})