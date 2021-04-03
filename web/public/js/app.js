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
	var updates = {};
	updates['/users/' + username + '/' + configForm.date.value + '/' + configForm.timeset.value] = configForm.duration.value;    
	firebase.database().ref().update(updates);
	configForm.reset();
	configModal.classList.remove('open');
	configForm.querySelector('.error').textContent = '';    
})