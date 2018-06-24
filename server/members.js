const Member = require('./member')

class Members {
	add(data){
		const m = new Member(Members.nextId++, data.name, data.sport)
		Members.members.push(m)
		return m
	}

	getAll(){
		return Members.members
	}

	get(id){
		const val =  Members.members.filter(m => m.id === id);
		console.log(val)
		if(val === null || val == null || val ===undefined){
			return null;
		}

		return val[0];
	}

	update(id, data){
		const m = this.get(id)
		if (m != null){
			m.name = data.name
			m.sport = data.sport
			return m
		}

		return null 
	}

	delete(id){
		const item = this.get(id)
		if(item != null){
			Members.members.splice(id,1)
			return true
		}
		return false
	}

}

Members.members = []
Members.nextId = 1

module.exports = Members