const setupList = (data) => {
    let configs = [];
    if (data){
        Object.keys(data).forEach(function(key, index){
          if (key.includes('config')){
            configs.push(data[key])
          }
        })
        let html = ``;
        console.log(configs);
        configs.forEach(config =>{
            html += `<li>${config.month} ${config.date},${config.year}</li>`
        })
        document.querySelector('ul').innerHTML = html;
    }
};
