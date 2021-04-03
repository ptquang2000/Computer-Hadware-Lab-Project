const setupList = (datas) => {
	let days = [];
	if (datas){
		Object.entries(datas).map(data => {
			if (!data[0].includes('id') && !data[0].includes('username'))
					days.push({...data[1], date: data[0]});
		})
		let html = ``;
		days.forEach(day =>{
			Object.entries(day).map(config => {
				if (!config[0].includes('date')){
					html += `<li config="${day.date}${config[0]}">
								<span class="text">${day.date} ${config[0]} </span>
								<div>
								<span class="text">${config[1]}</span>
								<button type="button" onclick="delFunc('${day.date}${config[0]}','${datas.username}')">delete</buton>
								</div>
								</li>`;
				}
			})
		})
		document.querySelector('ul').innerHTML = html;
	}
};

function delFunc(datetime, username) {
	firebase.database().ref('users/' + username + '/' + datetime.slice(0, -9) + '/' + datetime.slice(-9)).remove();
}