#include "Common.h"

#include <cstdint>

using namespace std;

namespace Shapes {

class Quadratic : public IShape {
public:
  Quadratic(ShapeType type) : type(type) {}
  unique_ptr<IShape> Clone() const final {
    auto Cloned = MakeShape(type);
    Cloned->SetPosition(this->GetPosition());
    Cloned->SetSize(this->GetSize());
    Cloned->SetTexture(this->texture);
    return move(Cloned);
  }

  void SetPosition(Point new_position) final {
    position = move(new_position);
  }
  Point GetPosition() const final {
    return position;
  }

  void SetSize(Size new_size) final {
    size = move(new_size);
  }
  Size GetSize() const final {
    return size;
  }

  void SetTexture(std::shared_ptr<ITexture> new_texture) final {
    texture = move(new_texture);
  }
  ITexture* GetTexture() const final {
    return texture.get();
  }

  void Draw(Image& columns) const override {
    Point position = GetPosition();
    Size size = GetSize();
    Size texture_size = {0, 0};
    ITexture* texture = GetTexture();
    if (texture != nullptr) {
      texture_size = texture->GetSize();
    }

    for (int y = 0; y < size.height && position.y + y < static_cast<int64_t>(columns.size()); y++) {
      auto& row = columns[position.y + y];
      for (int x = 0; x < size.width && position.x + x < static_cast<int64_t>(row.size()); x++) {
        if (DrawPoint({x, y})) {
          if (y < texture_size.height && x < texture_size.width) {
            row[position.x + x] = texture->GetImage()[y][x];
          }
          else {
            row[position.x + x] = '.';
          }
        }
      }
    }
  }
private:
  virtual bool DrawPoint(Point point) const = 0; 
  const ShapeType type;
  Point position;
  Size size;
  shared_ptr<ITexture> texture;
};

class Ellipse : public Quadratic {
public:
  Ellipse() : Quadratic(ShapeType::Ellipse) {}
private:
  bool DrawPoint(Point point) const override {
    return IsPointInEllipse(point, GetSize());
  }
};

class Rectangle : public Quadratic {
public:
  Rectangle() : Quadratic(ShapeType::Rectangle) {}
private:
  bool DrawPoint(Point) const override {
    return true;
  }
};

}

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  switch (shape_type) {
    case ShapeType::Ellipse:
      return make_unique<Shapes::Ellipse>();
    case ShapeType::Rectangle:
      return make_unique<Shapes::Rectangle>();
    default:
      throw invalid_argument("Unknown shape type");
  }
}