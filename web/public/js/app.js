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
    firebase.database().ref('users/' + username).child('total').get().then(function(snapshot) {
        if (snapshot.exists()) {

            var total = snapshot.val();
            var newConfig = {
                'month': configForm.month.value,
                'date': configForm.date.value,
                'year': configForm.year.value,
                'start': configForm.start.value,
                'stop': configForm.stop.value,
            };
            var updates = {};
            updates['/users/' + username + '/config' + total] = newConfig;
            updates['/users/' + username + '/total'] = total + 1;    
        
            firebase.database().ref().update(updates);
            configForm.reset();
            configModal.classList.remove('open');
            configForm.querySelector('.error').textContent = '';

        }
        else {
            configForm.querySelector('.error').textContent = "No data available";
        }
        }).catch(function(error) {
            configForm.querySelector('.error').textContent = error;
    });
    
})