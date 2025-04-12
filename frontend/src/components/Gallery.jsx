import { useState, useRef } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { useInView } from 'react-intersection-observer';
import { FaPlay, FaTimes, FaChevronLeft, FaChevronRight } from 'react-icons/fa';

const video = {
  id: 1,
  title: 'RayTracerNG Demo',
  description: 'Watch the engine in action with dynamic light reflections',
  thumbnail: 'https://img.youtube.com/vi/bKc2I06b_7U/maxresdefault.jpg',
  videoUrl: 'https://youtu.be/bKc2I06b_7U?si=C7jhQ9_PC2ws3BKU',
};

const images = [
  {
    id: 1,
    title: 'RayTracerNG Screenshot 1',
    description: 'Light emission and scene composition',
    url: 'https://github.com/Nischaya008/RayTracerNG/raw/main/RayTracerNG_Assets/Screenshot%202025-04-12%20175423.png',
  },
  {
    id: 2,
    title: 'RayTracerNG Screenshot 2',
    description: 'Light source and first-second reflections on obstacles',
    url: 'https://github.com/Nischaya008/RayTracerNG/raw/main/RayTracerNG_Assets/Screenshot%202025-04-12%20175501.png',
  },
  {
    id: 3,
    title: 'RayTracerNG Screenshot 3',
    description: 'Complex scene with dynamic lighting and reflections',
    url: 'https://github.com/Nischaya008/RayTracerNG/raw/main/RayTracerNG_Assets/Screenshot%202025-04-12%20175522.png',
  },
  {
    id: 4,
    title: 'RayTracerNG Screenshot 4',
    description: 'Single object with light source',
    url: 'https://github.com/Nischaya008/RayTracerNG/raw/main/RayTracerNG_Assets/Screenshot%202025-04-12%20175551.png',
  },
];

const VideoCard = ({ video, onPlay, index, totalItems }) => {
  const [ref, inView] = useInView({
    triggerOnce: true,
    threshold: 0.1,
  });

  return (
    <motion.div
      ref={ref}
      initial={{ opacity: 0, scale: 0.8 }}
      animate={inView ? { opacity: 1, scale: 1 } : { opacity: 0, scale: 0.8 }}
      transition={{ duration: 0.5 }}
      className="glass rounded-xl overflow-hidden group cursor-pointer w-full max-w-3xl mx-auto h-[300px] shadow-xl"
      onClick={() => onPlay(video)}
    >
      <div className="relative w-full h-full">
        <img
          src={video.thumbnail}
          alt={video.title}
          className="w-full h-full object-cover"
        />
        <div className="absolute inset-0 bg-black bg-opacity-40 flex items-center justify-center opacity-0 group-hover:opacity-100 transition-opacity">
          <FaPlay className="text-accent text-5xl" />
        </div>
      </div>
      <div className="absolute bottom-0 left-0 right-0 p-4 bg-black bg-opacity-70">
        <h3 className="text-xl font-display font-bold mb-1">{video.title}</h3>
        <p className="text-gray-300 text-xs">{video.description}</p>
      </div>
    </motion.div>
  );
};

const ImageCard = ({ image, onOpen, index, totalItems }) => {
  const [ref, inView] = useInView({
    triggerOnce: true,
    threshold: 0.1,
  });

  return (
    <motion.div
      ref={ref}
      initial={{ opacity: 0, scale: 0.8 }}
      animate={inView ? { opacity: 1, scale: 1 } : { opacity: 0, scale: 0.8 }}
      transition={{ duration: 0.5 }}
      className="glass rounded-lg overflow-hidden group flex-shrink-0 w-[120px] h-[90px] shadow-lg cursor-pointer"
      onClick={() => onOpen(image)}
    >
      <div className="relative w-full h-full">
        <img
          src={image.url}
          alt={image.title}
          className="w-full h-full object-cover"
        />
        <div className="absolute inset-0 bg-black bg-opacity-40 flex items-center justify-center opacity-0 group-hover:opacity-100 transition-opacity">
          <FaPlay className="text-accent text-2xl" />
        </div>
      </div>
    </motion.div>
  );
};

const VideoModal = ({ video, onClose }) => {
  // Extract YouTube video ID from the URL
  const getYouTubeId = (url) => {
    const regExp = /^.*(youtu.be\/|v\/|u\/\w\/|embed\/|watch\?v=|&v=)([^#&?]*).*/;
    const match = url.match(regExp);
    return (match && match[2].length === 11) ? match[2] : null;
  };

  const videoId = getYouTubeId(video.videoUrl);

  return (
    <motion.div
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      exit={{ opacity: 0 }}
      className="fixed inset-0 bg-black bg-opacity-90 z-50 flex items-center justify-center p-4"
      onClick={onClose}
    >
      <div className="relative max-w-4xl w-full">
        <button
          className="absolute -top-10 right-0 text-white hover:text-accent transition-colors"
          onClick={onClose}
        >
          <FaTimes size={24} />
        </button>
        {videoId ? (
          <iframe
            src={`https://www.youtube.com/embed/${videoId}`}
            title={video.title}
            className="w-full aspect-video rounded-lg"
            allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture"
            allowFullScreen
          ></iframe>
        ) : (
          <div className="w-full aspect-video rounded-lg bg-gray-800 flex items-center justify-center">
            <p className="text-white">Video not available</p>
          </div>
        )}
      </div>
    </motion.div>
  );
};

const ImageModal = ({ image, onClose, images, currentIndex, onNext, onPrevious }) => {
  const handleModalClick = (e) => {
    // Only close if clicking the background overlay
    if (e.target === e.currentTarget) {
      onClose();
    }
  };

  return (
    <motion.div
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      exit={{ opacity: 0 }}
      className="fixed inset-0 bg-black bg-opacity-90 z-50 flex items-center justify-center p-4"
      onClick={handleModalClick}
    >
      <div 
        className="relative max-w-4xl w-full"
        onClick={(e) => e.stopPropagation()}
      >
        <button
          className="absolute -top-10 right-0 text-white hover:text-accent transition-colors z-10"
          onClick={onClose}
        >
          <FaTimes size={24} />
        </button>

        {/* Navigation Buttons */}
        <button
          className="absolute left-0 top-1/2 -translate-y-1/2 text-white hover:text-accent transition-colors p-2 bg-black bg-opacity-50 rounded-full z-10"
          onClick={onPrevious}
          disabled={currentIndex === 0}
        >
          <FaChevronLeft size={24} />
        </button>

        <button
          className="absolute right-0 top-1/2 -translate-y-1/2 text-white hover:text-accent transition-colors p-2 bg-black bg-opacity-50 rounded-full z-10"
          onClick={onNext}
          disabled={currentIndex === images.length - 1}
        >
          <FaChevronRight size={24} />
        </button>

        <div className="relative w-full">
          <img
            src={image.url}
            alt={image.title}
            className="w-full h-auto rounded-lg"
          />
          <div className="absolute bottom-0 left-0 right-0 p-4 bg-black bg-opacity-70 rounded-b-lg">
            <p className="text-gray-300 text-sm text-center">{image.description}</p>
          </div>
        </div>
      </div>
    </motion.div>
  );
};

const Gallery = () => {
  const [selectedVideo, setSelectedVideo] = useState(null);
  const [selectedImage, setSelectedImage] = useState(null);
  const [currentImageIndex, setCurrentImageIndex] = useState(0);

  const handleImageOpen = (image) => {
    const index = images.findIndex(img => img.id === image.id);
    setCurrentImageIndex(index);
    setSelectedImage(image);
  };

  const handleNextImage = (e) => {
    e?.stopPropagation();
    if (currentImageIndex < images.length - 1) {
      const nextIndex = currentImageIndex + 1;
      setCurrentImageIndex(nextIndex);
      setSelectedImage(images[nextIndex]);
    }
  };

  const handlePreviousImage = (e) => {
    e?.stopPropagation();
    if (currentImageIndex > 0) {
      const prevIndex = currentImageIndex - 1;
      setCurrentImageIndex(prevIndex);
      setSelectedImage(images[prevIndex]);
    }
  };

  const handleCloseImage = () => {
    setSelectedImage(null);
  };

  return (
    <section id="gallery" className="section bg-secondary py-20">
      <div className="container">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
          className="text-center mb-16"
        >
          <h2 className="text-4xl md:text-5xl font-display font-bold mb-4">
            Gallery
          </h2>
          <p className="text-gray-300 text-lg">
            Watch RayTracerNG in action
          </p>
        </motion.div>

        {/* Main Video Section */}
        <div className="mb-8">
          <VideoCard 
            video={video} 
            onPlay={setSelectedVideo} 
            index={0} 
            totalItems={1} 
          />
        </div>

        {/* Images Section */}
        <div className="max-w-4xl mx-auto">
          <div className="flex justify-center gap-3">
            {images.map((image, idx) => (
              <div key={image.id} className="flex items-center justify-center">
                <ImageCard 
                  image={image} 
                  onOpen={handleImageOpen}
                  index={idx} 
                  totalItems={images.length} 
                />
              </div>
            ))}
          </div>
        </div>

        <AnimatePresence>
          {selectedVideo && (
            <VideoModal
              video={selectedVideo}
              onClose={() => setSelectedVideo(null)}
            />
          )}
          {selectedImage && (
            <ImageModal
              image={selectedImage}
              onClose={handleCloseImage}
              images={images}
              currentIndex={currentImageIndex}
              onNext={handleNextImage}
              onPrevious={handlePreviousImage}
            />
          )}
        </AnimatePresence>
      </div>
    </section>
  );
};

export default Gallery; 