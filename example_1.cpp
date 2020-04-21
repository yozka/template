#include <memory>
#include <functional>
#include <vector>
#include <map>

#include <fstream>
#include <iostream>
///--------------------------------------------------------------------



/*
* создание объектов. круга, полигоны итд.
* их сериализация и десериализация
* фабрика объектов
* актульано до 2019 год
*/


 ///-------------------------------------------------------------------
///
/// <summary>
/// struct Point
/// </summary>
///
///--------------------------------------------------------------------
struct Point
{
	int x = { 0 };
	int y = { 0 };

	Point(int i_x = {}, int i_y = {}) noexcept : x(i_x), y(i_y) {}

	bool operator == (const Point &pt) const noexcept
	{
		return x == pt.x && y == pt.y;
	}
};
///--------------------------------------------------------------------




 ///-------------------------------------------------------------------
///
/// <summary>
/// type objects
/// </summary>
///
///--------------------------------------------------------------------
enum ETypeShape : int
{
	circle	= 1,
	polygon = 2
};
///--------------------------------------------------------------------






 ///-------------------------------------------------------------------
///
/// <summary>
/// base shape
/// </summary>
///
///--------------------------------------------------------------------
class AShape
{
public:
	virtual ~AShape() = default; 
	virtual void draw() = 0; 
	///--------------------------------------------------------------------




	 ///-------------------------------------------------------------------
	///
	/// <summary>
	/// type objects for serialization
	/// </summary>
	///
	///--------------------------------------------------------------------
	virtual ETypeShape type()const = 0;
	///--------------------------------------------------------------------





	 ///-------------------------------------------------------------------
	///
	/// <summary>
	/// comparing shape objects
	/// </summary>
	///
	///--------------------------------------------------------------------
	virtual bool isEqual(const std::shared_ptr<AShape> &shape) const = 0;
	///--------------------------------------------------------------------


	
	 ///-------------------------------------------------------------------
	///
	/// <summary>
	/// serialization object
	/// </summary>
	///
	///--------------------------------------------------------------------
	std::ostream& serialize(std::ostream& os) const
	{
		const auto shapeType = type();
		os.write(reinterpret_cast<const char*>(&shapeType), sizeof(ETypeShape));
		return serializeState(os);
	}
	///--------------------------------------------------------------------



	
	 ///-------------------------------------------------------------------
	///
	/// <summary>
	/// Internal state serialization
	/// </summary>
	///
	///--------------------------------------------------------------------
	virtual std::ostream& serializeState(std::ostream& os) const = 0;
	///--------------------------------------------------------------------





	 ///-------------------------------------------------------------------
	///
	/// <summary>
	/// Internal state deserialization
	/// </summary>
	///
	///--------------------------------------------------------------------
	virtual std::istream& deserializeState(std::istream& is) = 0;
	///--------------------------------------------------------------------



	
	 ///-------------------------------------------------------------------
	///
	/// <summary>
	/// type deserialization
	/// </summary>
	///
	///--------------------------------------------------------------------
	static ETypeShape typeDeserialize(std::istream& is)
	{
		ETypeShape typeShape;
		is.read(reinterpret_cast<char*>(&typeShape), sizeof(ETypeShape));
		return typeShape;
	}
	///--------------------------------------------------------------------


protected:

	AShape() = default;

};
///--------------------------------------------------------------------
using PShape = std::shared_ptr<AShape>;
///--------------------------------------------------------------------









 ///-------------------------------------------------------------------
///
/// <summary>
/// Circle shape
/// </summary>
///
///--------------------------------------------------------------------
class ACircle
	:
	public AShape
{
public:


	ACircle(const Point &center = {}, const int radius = {})
		:
		m_center(center),
		m_radius(radius)
	{
	}


	ETypeShape type()const override 
	{ 
		return ETypeShape::circle; 
	}



	bool isEqual(const PShape &shape) const override
	{
		if (shape->type() != ETypeShape::circle)
		{
			return false;
		}
		const auto obj = std::dynamic_pointer_cast<ACircle>(shape);
		if (!obj)
		{
			return false;
		}
		return m_radius == obj->m_radius && m_center == obj->m_center;
	}



	void draw() override
	{

	}


	std::ostream& serializeState(std::ostream& os) const override
	{
		os.write(reinterpret_cast<const char*>(&m_center), sizeof(Point));
		os.write(reinterpret_cast<const char*>(&m_radius), sizeof(int));
		return os;
	}


	std::istream& deserializeState(std::istream& is) override
	{
		is.read(reinterpret_cast<char*>(&m_center), sizeof(Point));
		is.read(reinterpret_cast<char*>(&m_radius), sizeof(int));
		return is;
	}


private:

	Point	m_center = { 0, 0 };
	int		m_radius = { 0 };
};
///--------------------------------------------------------------------
using PCircle = std::shared_ptr<ACircle>;
///--------------------------------------------------------------------






 ///-------------------------------------------------------------------
///
/// <summary>
/// Polygon shape
/// </summary>
///
///--------------------------------------------------------------------
class APolygon
	:
	public AShape
{
public:


	APolygon(const std::vector<Point> &vertices = {})
		:
		m_vertices(vertices)
	{
	}


	ETypeShape type()const override
	{
		return ETypeShape::polygon;
	}


	bool isEqual(const PShape &shape) const override
	{
		if (shape->type() != ETypeShape::polygon)
		{
			return false;
		}
		const auto obj = std::dynamic_pointer_cast<APolygon>(shape);
		if (!obj)
		{
			return false;
		}
		
		if (m_vertices != obj->m_vertices)
		{
			return false;
		}
		return true;
	}


	void draw() override
	{
	}


	std::ostream& serializeState(std::ostream& os) const override
	{
		const size_t count = m_vertices.size();
		os.write(reinterpret_cast<const char*>(&count), sizeof(size_t));

		for (const auto &point : m_vertices)
		{
			os.write(reinterpret_cast<const char*>(&point), sizeof(Point));
		}
		return os;
	}



	std::istream& deserializeState(std::istream& is) override
	{
		size_t count = 0;
		is.read(reinterpret_cast<char*>(&count), sizeof(size_t));

		m_vertices.clear();
		m_vertices.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			Point point;
			is.read(reinterpret_cast<char*>(&point), sizeof(Point));
			m_vertices.push_back(point);
		}
		return is;
	}
	//

private:
	std::vector<Point> m_vertices;
};
///--------------------------------------------------------------------
using PPolygon = std::shared_ptr<APolygon>;
///--------------------------------------------------------------------










 ///-------------------------------------------------------------------
///
/// <summary>
/// Factory to create objects 
/// </summary>
///
///--------------------------------------------------------------------
class AFactory
{
public:
	///--------------------------------------------------------------------




	///--------------------------------------------------------------------
	template<class T, class... _Types>
	inline std::shared_ptr<T> create(_Types&&... _args)
	{
		return std::make_shared<T>(_STD forward<_Types>(_args)...);
	}
	///--------------------------------------------------------------------





	///--------------------------------------------------------------------
	template<class... _Types>
	inline PCircle createCircle(_Types&&... _args)
	{
		return create<ACircle>(_STD forward<_Types>(_args)...);
	}
	///--------------------------------------------------------------------




	///--------------------------------------------------------------------
	template<class... _Types>
	inline PPolygon createPolygon(_Types&&... _args)
	{
		return create<APolygon>(_STD forward<_Types>(_args)...);
	}
	///--------------------------------------------------------------------







	 ///-------------------------------------------------------------------
	///
	/// <summary>
	/// Create objects from type
	/// </summary>
	///
	///--------------------------------------------------------------------
	PShape create(const ETypeShape typeShape)
	{
		static const std::map<ETypeShape, std::function<PShape()>> factory_map
		{
			{ ETypeShape::circle,  [this] {return  createCircle(); } },
			{ ETypeShape::polygon, [this] {return  createPolygon(); } }
		};

		const auto it = factory_map.find(typeShape);
		if (it != factory_map.end())
		{
			return it->second();
		}

		return PShape(); //or need throw?
	}
	///--------------------------------------------------------------------




	 ///-------------------------------------------------------------------
	///
	/// <summary>
	/// Create objects from stream
	/// </summary>
	///
	///--------------------------------------------------------------------
	PShape create(std::istream& is)
	{
		const auto type = AShape::typeDeserialize(is);
		PShape shape = create(type);
		if (shape)
		{
			shape->deserializeState(is);
		}
		return shape;
	}
	///--------------------------------------------------------------------




};
///------------------------------------------------------------------------






int main()
{

	std::vector<PShape> dataStored;
	std::vector<PShape> dataRestored;


	//Create 10 random different Shapes;
	{
		AFactory factory;

		//factory use example
		auto shape1 = factory.create<ACircle>();
		auto shape2 = factory.create<ACircle>(Point{ rand(), rand() }, 34);
		auto shape3 = factory.createCircle();
		auto shape4 = factory.createCircle(Point{ 45, 24 }, 34);

		auto shape5 = factory.create<APolygon>();
		auto shape6 = factory.create<APolygon>(std::vector<Point>{ {40, 41}, { 50,51 }});
		auto shape7 = factory.createPolygon();
		auto shape8 = factory.createPolygon(std::vector<Point>{ {10, 11}, { 20,21 }, { 30, 31 }});

		//factory for deserialization objects 
		auto shape9	 = factory.create(ETypeShape::circle);
		auto shape10 = factory.create(ETypeShape::polygon);
		//


		for (int i = 0; i < 10; i++)
		{
			PShape shape;
			if (rand() % 2 == 0)
			{
				//create circle
				shape = factory.createCircle(Point{ rand(), rand() }, 20);
			}
			else
			{
				//create polygon
				std::vector<Point> vertices;
				const int count = rand() % 10 + 2;
				for (int j = 0; j < count; j++)
				{
					vertices.push_back(Point{ rand(), rand() });
				}
				shape = factory.createPolygon(vertices);
				//
			}

			if (shape)
			{
				dataStored.push_back(shape);
			}
		}
	}
	//




	//save it on disk 
	{
		std::ofstream file;
		file.open("temp.dat", std::ios::binary);
		for (const auto &shape : dataStored)
		{
			shape->serialize(file);
		}
		file.flush();
		file.close();
	}

	//read from disk
	{
		std::ifstream file;
		file.open("temp.dat", std::ios::binary);

		AFactory factory;
		while (!file.eof())
		{
			auto shape = factory.create(file);
			if (shape)
			{
				dataRestored.push_back(shape);
			}
		}
	}
	//




	//check 
	auto itemS = dataStored.cbegin();
	auto itemR = dataRestored.cbegin();
	while (	itemS != dataStored.cend() && 
			itemR != dataRestored.cend())
	{
		if (!(*itemR)->isEqual(*itemS))
		{
			throw "NOT COMPARE!!!";
		}
		++itemS;
		++itemR;
	}
	return 0;
}


