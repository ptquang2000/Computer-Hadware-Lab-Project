const configModal = document.querySelector('.new-config');
const configLink = document.querySelector('.add-config');

configLink.addEventListener('click', ()=>{
    configModal.classList.add('open')
})

configModal.addEventListener('click', (e)=>{
    if (e.target.classList.contains('new-config')){
        configModal.classList.remove('open')
    }
})