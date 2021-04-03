const setupList = (datas) => {
    let configs = [];
    if (datas){
        Object.entries(datas).map(data => {
            if (!data[0].includes('total') && !data[0].includes('id') && !data[0].includes('username'))
              configs.push({...data[1], id: data[0]});
          })
        
        let html = ``;
        configs.forEach(config =>{
            html += `<li id="${config.id}">
            <span class="text">${config.month} ${config.date},${config.year}</span>
            <div>
              <span class="time">${config.start}-${config.stop}</span>
              <button type="button" onclick="delFunc('${config.id}','${datas.username}')">delete</buton>
            </div>
          </li>`;
        })
        document.querySelector('ul').innerHTML = html;
    }
};

function delFunc(config_id, username) {
  firebase.database().ref('users/' + username + '/' + config_id).remove();
}