const setupList = (datas) => {
    let configs = [];
    if (datas){
        Object.entries(datas).map(data => {
            if (!data[0].includes('total') && !data[0].includes('id'))
            configs.push({...data[1], id: data[0]});
          })
          
        let html = ``;
        configs.forEach(config =>{
            html += `<li id="${config.id}">
            <span class="text">${config.month} ${config.date},${config.year}</span>
            <div>
              <span class="time">${config.start}-${config.stop}</span>
              <button type="button" onclick="alert('${config.id}')">delete</buton>
            </div>
          </li>`;
        })
        document.querySelector('ul').innerHTML = html;
    }
};